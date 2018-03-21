#include "authoritative_game.hpp"
#include "../common/actor/unit.hpp"
#include "../common/datadriven/data_list.hpp"
#include "../common/networking/packet.hpp"
#include "../common/networking/world_map.hpp"
#include "../common/networking/world_chunk.hpp"

#include <thread>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

// The states:
//  - lobby
//  - gameplay

authoritative_game::authoritative_game()
: base_game(std::thread::hardware_concurrency() - 1)
, world(static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count()))
, sockets(3) {

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
    for(int i = 0; i < 10 && !connection_listener.try_bind(6426); ++i) {
        std::cerr << " attempt # " << i + 1 << " to bind port failed because: " << SDLNet_GetError() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if(connection_listener.is_bound()) {
        std::cout << "waiting for connections on port 6426" << std::endl;
        sockets.add(connection_listener);
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

void authoritative_game::on_connection() {
    // Get client socket
    networking::tcp_socket connecting_socket = connection_listener.accept();

    networking::world_map map_infos(20, 20);


    if(networking::send_packet(connecting_socket, networking::packet::make(map_infos))) {
        for(const world_chunk& chunk : world) {
            std::vector<uint8_t> biomes;
            biomes.reserve(world::CHUNK_WIDTH * world::CHUNK_HEIGHT * world::CHUNK_DEPTH);

            // SETUP BIOMES
            for(int y = 0; y < world::CHUNK_HEIGHT; ++y) {
                for(int z = 0; z < world::CHUNK_DEPTH; ++z) {
                    for(int x = 0; x < world::CHUNK_WIDTH; ++x) {
                        biomes.push_back(chunk.biome_at(x, y, z));
                    }
                }
            }

            networking::world_chunk chunk_info(chunk.position().x, chunk.position().y, biomes);
            networking::send_packet(connecting_socket, networking::packet::make(chunk_info));
        }
    }

    // TODO: Reserve a place to current connection
    // TODO: CRYPTO: Send public key to client
    // TODO: CRYPTO: Wait for symetric key from client
    // TODO: Send this client the flyweights

    // Add the client
    sockets.add(connecting_socket);
    connected_clients.push_back(std::move(connecting_socket));
}

void authoritative_game::on_client_data(const client& c) {
    // TODO: Handle request in worker thread
    auto received_packet = networking::receive_packet_from(c);

    if(received_packet) {
        try {
            nlohmann::json json_body = nlohmann::json::parse(std::begin(received_packet->bytes),
                                                             std::end(received_packet->bytes));

            // here we should have a valid json
        }
        catch(const nlohmann::json::parse_error& e) {
            // TODO: The client has sent us crap
            std::cerr << "client has sent us some crap: " << "[" << e.id << "] " << e.what() << std::endl;
        }
    }
    else {
        on_client_disconnection(c);
    }

    // TODO: Read what the user has sent
    // TODO: Validate input
}

void authoritative_game::on_client_disconnection(const client& c) {
    sockets.remove(c);

    // Remove the client from the connected clients
    auto it = std::find(std::begin(connected_clients), std::end(connected_clients), c);
    connected_clients.erase(it);

    // TODO: Determine how to handle a player disconnection in game
}

void authoritative_game::check_sockets() {
    int numready = sockets.check(std::chrono::milliseconds(0));
    if(numready == -1) {
        std::cerr << "SDLNet_CheckSockets: " << SDLNet_GetError() << std::endl;
    }
    else if(numready > 0){
        if(sockets.is_ready(connection_listener)) {
            on_connection();
        }
        else {
            std::for_each(std::begin(connected_clients), std::end(connected_clients), [this](const client& c) {
                if(sockets.is_ready(c)) {
                    on_client_data(c);
                }
            });
        }
    }
}

void authoritative_game::on_update(frame_duration last_frame) {
    std::chrono::milliseconds last_frame_ms = std::chrono::duration_cast<std::chrono::milliseconds>(last_frame);

    // Handle data reception from clients
    check_sockets();

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
}

void authoritative_game::on_release() {
    std::cout << "releasing..." << std::endl;
    std::for_each(std::begin(connected_clients), std::end(connected_clients), [this](const networking::tcp_socket& socket) {
        sockets.remove(socket);
    });

    connected_clients.clear();
}