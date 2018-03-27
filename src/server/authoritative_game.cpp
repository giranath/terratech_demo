#include "authoritative_game.hpp"
#include "../common/actor/unit.hpp"
#include "../common/datadriven/data_list.hpp"
#include "../common/networking/packet.hpp"
#include "../common/networking/world_map.hpp"
#include "../common/networking/world_chunk.hpp"
#include "../common/networking/networking_constant.hpp"

#include <thread>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include "server_unit_manager.hpp"

// The states:
//  - lobby
//  - gameplay

uint8_t authoritative_game::client::next_id = 0;

authoritative_game::authoritative_game()
: base_game(std::thread::hardware_concurrency() - 1, std::make_unique<server_unit_manager>())
, world(static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count()))
, network(3) {

}

void authoritative_game::load_flyweights() {
    std::ifstream units_list_stream("asset/data/unit.list");
    if(!units_list_stream.is_open()) {
        std::cerr << "cannot open 'asset/data/unit.list'" << std::endl;
    }

    data::load_data_list<std::string>(units_list_stream, [this](const std::string& rel_path) {
        std::string full_path = "asset/data/" + rel_path;

        std::ifstream unit_stream(full_path);
        if(unit_stream.is_open()) {
            std::cout << "  loading flyweight '" << full_path << "'" << std::endl;
            load_flyweight(nlohmann::json::parse(unit_stream));
        }
        else {
            std::cerr << "  cannot open " << full_path << std::endl;
        }
    });
}

void authoritative_game::load_assets() {
    std::cout << "loading assets..." << std::endl;
    load_flyweights();
    // TODO: Load building
    // TODO: Load world generation
}

void authoritative_game::generate_world() {
    std::cout << "generating world..." << std::endl;
    for(std::size_t x = 0; x < 20; ++x) {
        for(std::size_t z = 0; z < 20; ++z) {
            world.generate_at(x, z);
        }
    }

    // TODO: Caculate best starting spots
    // Using the thread pool, calculate a score for each individual chunk based on biomes + sites
    //                        then, accumulate a chunk's score with it's neighbours
    //                        then, try to find two biomes with similar scores at a certain distance
}

void authoritative_game::setup_listener() {
    std::cout << "binding to port 6426..." << std::endl;
    network.load_rsa_keys("asset/crypto/privkey.p8", "asset/crypto/pubkey.der");
    network.on_connection.attach([this](networking::network_manager::socket_handle connected) {
        std::cout << connected << " has connected" << std::endl;
        on_connection(connected);
    });
    network.on_disconnection.attach([this](networking::network_manager::socket_handle disconnected) {
        std::cout << disconnected << " has disconnected" << std::endl;

        std::lock_guard<std::mutex> lock(clients_mutex);
        auto it = std::find_if(std::begin(connected_clients), std::end(connected_clients), [disconnected](const client& c) {
            return c.socket == disconnected;
        });

        if(it != std::end(connected_clients)) {
            connected_clients.erase(it);
        }
    });

    for(int i = 0; i < 10 && !network.try_bind(6426); ++i) {
        std::cerr << " attempt # " << i + 1 << " to bind port failed because: " << SDLNet_GetError() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if(network.is_bound()) {
        std::cout << "waiting for connections on port 6426" << std::endl;
    }
    else {
        throw std::runtime_error("failed to bind port");
    }
}

void authoritative_game::on_init() {
    load_assets();
    generate_world();

    setup_listener();
}

void authoritative_game::on_connection(networking::network_manager::socket_handle handle) {
    client connected_client(handle);
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        connected_clients.push_back(connected_client);
    }

    // Send the flyweights
    std::cout << "sending flyweights..." << std::endl;
    std::unordered_map<std::string, unit_flyweight> serialized_flyweights;
    for(auto it = unit_flyweights().begin(); it != unit_flyweights().end(); ++it) {
        serialized_flyweights.emplace(std::to_string(it->first), it->second);
    }
    network.send_to(networking::packet::make(serialized_flyweights, PACKET_SETUP_FLYWEIGHTS), handle);

    // Send the map
    std::cout << "sending map..." << std::endl;
    std::vector<networking::world_chunk> chunks_to_send;
    chunks_to_send.reserve(std::distance(world.begin(), world.end()));

    std::transform(world.begin(), world.end(), std::back_inserter(chunks_to_send), [](const world_chunk& chunk) {
        std::vector<uint8_t> biomes;
        biomes.reserve(world::CHUNK_WIDTH * world::CHUNK_HEIGHT * world::CHUNK_DEPTH);

        // SETUP BIOMES
        for(uint32_t y = 0; y < world::CHUNK_HEIGHT; ++y) {
            for(uint32_t z = 0; z < world::CHUNK_DEPTH; ++z) {
                for(uint32_t x = 0; x < world::CHUNK_WIDTH; ++x) {
                    biomes.push_back(static_cast<uint8_t>(chunk.biome_at(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z))));
                }
            }
        }

        return networking::world_chunk(chunk.position().x, chunk.position().y, biomes);
    });

    network.send_to(networking::packet::make(chunks_to_send, PACKET_SETUP_CHUNK), handle);

    spawn_unit(connected_client.id, glm::vec3(0.f, 0.f, 0.f), glm::vec2{1000.f, 1000.f}, 106);
}

void authoritative_game::spawn_unit(uint8_t owner, glm::vec3 position, glm::vec2 target, int flyweight_id) {
    unit_manager& manager = units();
    server_unit_manager& units = static_cast<server_unit_manager&>(manager);
    auto created_unit = units.add_unit_to(owner, make_unit(position, target, flyweight_id));

    std::vector<unit> units_to_spawn;
    units_to_spawn.push_back(*static_cast<unit*>(created_unit.get()));

    network.broadcast(networking::packet::make(units_to_spawn, PACKET_SPAWN_UNITS));
}

void authoritative_game::on_update(frame_duration last_frame) {
    static frame_duration acc;
    std::chrono::milliseconds last_frame_ms = std::chrono::duration_cast<std::chrono::milliseconds>(last_frame);

    auto received_packets = network.poll_packets();
    // TODO: Handle received packets

    auto update_task = push_task(async::make_task([this, last_frame_ms]() {
        for (auto u = units().begin_of_units(); u != units().end_of_units(); u++) {
            unit* actual_unit = static_cast<unit*>(u->second.get());

            glm::vec2 target = actual_unit->get_target_position();
            glm::vec3 target3D = { target.x, 0, target.y };

            glm::vec3 direction = target3D - actual_unit->get_position();

            if (direction == glm::vec3{}) continue;

            direction = glm::normalize(direction);

            glm::vec3 move = actual_unit->get_position() + (direction * 100.0f * (last_frame_ms.count() / 1000.0f));

            actual_unit->set_position(move);
        }
    }));

    update_task.wait();

    acc += last_frame;

    if(acc > std::chrono::seconds(1)) {
        std::vector<unit> updated_units;
        std::transform(units().begin_of_units(), units().end_of_units(), std::back_inserter(updated_units),
                       [](const auto &p) {
                           return *static_cast<unit *>(p.second.get());
                       });
        // Send unit update every frame
        if(updated_units.size() > 0) {
            network.broadcast(networking::packet::make(updated_units, PACKET_UPDATE_UNITS));
        }
        acc = std::chrono::seconds(0);
    }
}

void authoritative_game::on_release() {
    /*
    std::cout << "releasing..." << std::endl;
    std::for_each(std::begin(connected_clients), std::end(connected_clients), [this](const client& client) {
        sockets.remove(client.socket);
    });

    connected_clients.clear();
     */
}