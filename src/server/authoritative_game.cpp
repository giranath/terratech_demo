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
#include "../common/networking/update_target.hpp"
#include "../common/task/update_player_visibility.hpp"
#include "../common/task/update_units.hpp"
#include "../common/networking/player_init.hpp"

// The states:
//  - lobby
//  - gameplay

// TODO:
// Keep track of what each player can see

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

class chunk_score_task : public async::base_task {
    const world_chunk& chunk_;
    double score_;
protected:
    void set_score(double value) {
        score_ = value;
    }
public:
    chunk_score_task(const world_chunk& chunk)
    : chunk_(chunk)
    , score_(0.0) {

    }

    double score() const noexcept {
        return score_;
    }

    const world_chunk& chunk() const {
        return chunk_;
    }
};

class chunk_score_calculator_task : public chunk_score_task {
public:
    explicit chunk_score_calculator_task(const world_chunk& chunk)
    : chunk_score_task(chunk) {

    }

    void execute() override {
        set_score(chunk().score());
    }
};

class starting_point_finding_task : public async::base_task {
public:
    using iterator = std::unordered_map<std::size_t, std::vector<std::size_t>>::const_iterator;
private:
    std::pair<std::size_t, std::size_t> found_pair;
    const std::vector<std::tuple<int, int, double>>& region_scores;
    iterator begin;
    iterator end;
public:
    starting_point_finding_task(iterator begin, iterator end, const std::vector<std::tuple<int, int, double>>& regions)
    : region_scores(regions)
    , begin(begin)
    , end(end) {

    }

    void execute() override {
        double bigest_score = -1.0;
        std::for_each(begin, end, [this, &bigest_score](const auto& pair) {
            const double score_to_match = std::get<2>(region_scores[pair.first]);

            if(score_to_match > bigest_score) {
                auto best_match = std::min_element(std::begin(pair.second), std::end(pair.second),
                                                   [this, score_to_match](std::size_t a, std::size_t b) {
                                                       const double score_diff_a = std::abs(
                                                               score_to_match - std::get<2>(region_scores[a]));
                                                       const double score_diff_b = std::abs(
                                                               score_to_match - std::get<2>(region_scores[b]));

                                                       return score_diff_a < score_diff_b;
                                                   });
                auto best_match_pair = std::make_pair(pair.first, *best_match);
                std::swap(found_pair, best_match_pair);
                bigest_score = score_to_match;
            }
        });
    }

    std::pair<std::size_t, std::size_t> found() const noexcept {
        return found_pair;
    }
};

void authoritative_game::find_spawn_chunks() {
    std::vector<async::task_executor::task_future> chunk_scores_tasks;
    std::transform(std::begin(world), std::end(world), std::back_inserter(chunk_scores_tasks), [this](const world_chunk& chunk) {
        return push_task(std::make_unique<chunk_score_calculator_task>(chunk));
    });

    // TODO: Do other stuff here

    std::unordered_map<glm::i32vec2, double, util::vec2_hash<glm::i32vec2>> chunk_scores;
    for(async::task_executor::task_future& future : chunk_scores_tasks) {
        async::task_executor::task_ptr task = future.get();
        chunk_score_calculator_task* calculated_score = static_cast<chunk_score_calculator_task*>(task.get());
        chunk_scores.emplace(glm::i32vec2(calculated_score->chunk().position().x, calculated_score->chunk().position().y), calculated_score->score());
    }

    std::vector<std::tuple<int, int, double>> region_scores;
    std::for_each(std::begin(chunk_scores), std::end(chunk_scores), [&chunk_scores, &region_scores](const std::pair<glm::i32vec2, double>& pair) {
        std::vector<double> n;

        if(pair.first.x > 0) {
            n.push_back(chunk_scores[{pair.first.x - 1, pair.first.y}]);

            if(pair.first.y > 0) {
                n.push_back(chunk_scores[{pair.first.x - 1, pair.first.y - 1}]);
            }

            if(pair.first.y < 20) {
                n.push_back(chunk_scores[{pair.first.x - 1, pair.first.y + 1}]);
            }
        }

        if(pair.first.y > 0) {
            n.push_back(chunk_scores[{pair.first.x, pair.first.y - 1}]);
        }

        if(pair.first.x < 20) {
            n.push_back(chunk_scores[{pair.first.x + 1, pair.first.y}]);

            if(pair.first.y > 0) {
                n.push_back(chunk_scores[{pair.first.x + 1, pair.first.y - 1}]);
            }

            if(pair.first.y < 20) {
                n.push_back(chunk_scores[{pair.first.x + 1, pair.first.y + 1}]);
            }
        }

        if(pair.first.y < 20) {
            n.push_back(chunk_scores[{pair.first.x, pair.first.y + 1}]);
        }

        region_scores.emplace_back(pair.first.x, pair.first.y, std::accumulate(std::begin(n), std::end(n), pair.second));
    });

    std::sort(std::begin(region_scores), std::end(region_scores), [](const std::tuple<int, int, double>& a, const std::tuple<int, int, double>& b) {
        return std::get<2>(a) < std::get<2>(b);
    });

    std::size_t chunk_a_index = 0;
    std::size_t chunk_b_index = 1;

    // Associate each chunk with other chunk to check
    std::unordered_map<std::size_t, std::vector<std::size_t>> indices_to_check;

    for(std::size_t i = 0; i < region_scores.size(); ++i) {
        for(std::size_t j = 0; j < region_scores.size(); ++j) {
            if(i != j) {
                // Check if an association may be done
                const glm::vec2 chunk_a_pos(std::get<0>(region_scores[i]), std::get<1>(region_scores[i]));
                const glm::vec2 chunk_b_pos(std::get<0>(region_scores[j]), std::get<1>(region_scores[j]));

                float dist = glm::length(chunk_b_pos - chunk_a_pos);
                // Players must be separated by at least 5 chunks
                if(dist > 10.f) {
                    indices_to_check[i].push_back(j);
                }
            }
        }
    }

    // Splits the load on 4 threads
    const std::size_t BUCKET_COUNT = 4;
    const std::size_t BUCKET_SIZE = (indices_to_check.size() / BUCKET_COUNT) + 1;
    std::size_t missing_count = indices_to_check.size();

    // Create 4 processing batch
    std::vector<async::task_executor::task_future> bucket_futures;
    auto begin_of_bucket = std::begin(indices_to_check);
    for(std::size_t i = 0; i < BUCKET_COUNT; ++i) {
        const std::size_t size = std::min(BUCKET_SIZE, missing_count);

        auto end_of_bucket = std::next(begin_of_bucket, size);
        bucket_futures.push_back(push_task(std::make_unique<starting_point_finding_task>(std::begin(indices_to_check),
                                                                                         std::end(indices_to_check),
                                                                                         region_scores)));

        begin_of_bucket = end_of_bucket;
        missing_count -= size;
    }

    // Wait every batch to be processed
    std::vector<std::pair<std::size_t, std::size_t>> best_pairs;
    std::transform(std::begin(bucket_futures), std::end(bucket_futures), std::back_inserter(best_pairs), [](async::task_executor::task_future& future) {
        auto task = future.get();
        starting_point_finding_task* finding_task = static_cast<starting_point_finding_task*>(task.get());

        return finding_task->found();
    });

    // Find best score in matching points
    auto best_pair = std::max_element(std::begin(best_pairs), std::end(best_pairs),
                                      [&region_scores](const std::pair<std::size_t, std::size_t>& a,
                                                       const std::pair<std::size_t, std::size_t>& b) {
                                          return std::get<2>(region_scores[a.first]) < std::get<2>(region_scores[b.first]);
                                      });

    spawn_chunks[0] = glm::i32vec2(std::get<0>(region_scores[best_pair->first]), std::get<1>(region_scores[best_pair->first]));
    spawn_chunks[1] = glm::i32vec2(std::get<0>(region_scores[best_pair->second]), std::get<1>(region_scores[best_pair->second]));
}

void authoritative_game::generate_world() {
    std::cout << "generating world..." << std::endl;

    for(std::size_t x = 0; x < 20; ++x) {
        for(std::size_t z = 0; z < 20; ++z) {
            world_chunk& chunk = world.generate_at(x, z);
        }
    }

    find_spawn_chunks();
}

void authoritative_game::setup_listener() {
    std::cout << "binding to port 6426..." << std::endl;
    network.load_rsa_keys("asset/crypto/privkey.p8", "asset/crypto/pubkey.der");
    network.load_certificate("asset/crypto/privcertificate.p8", "asset/crypto/pubcertificate.der");

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
            std::cout << "stopping server" << std::endl;
            stop();
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

void authoritative_game::send_flyweights(networking::network_manager::socket_handle client) {
    std::cout << "sending flyweights..." << std::endl;
    std::unordered_map<std::string, unit_flyweight> serialized_flyweights;
    for(auto it = unit_flyweights().begin(); it != unit_flyweights().end(); ++it) {
        serialized_flyweights.emplace(std::to_string(it->first), it->second);
    }
    network.send_to(networking::packet::make(serialized_flyweights, PACKET_SETUP_FLYWEIGHTS), client);
}

void authoritative_game::send_map(const client& connecting_client) {
    std::cout << "sending map..." << std::endl;
    world::chunk_collection filtered_chunks;
    std::copy_if(std::begin(world), std::end(world), std::back_inserter(filtered_chunks), [&connecting_client](const world_chunk& chunk) {
       return connecting_client.known_chunks.find(chunk.position()) != std::end(connecting_client.known_chunks);
    });

    std::vector<networking::world_chunk> chunks_to_send;
    chunks_to_send.reserve(std::distance(world.begin(), world.end()));
    std::transform(std::begin(filtered_chunks), std::end(filtered_chunks), std::back_inserter(chunks_to_send), [&connecting_client](const world_chunk& chunk) {
        std::vector<uint8_t> biomes;
        biomes.reserve(world::CHUNK_WIDTH * world::CHUNK_HEIGHT * world::CHUNK_DEPTH);

        std::vector<networking::resource> resources;
        // SETUP BIOMES
        for(uint32_t y = 0; y < world::CHUNK_HEIGHT; ++y) {
            for(uint32_t z = 0; z < world::CHUNK_DEPTH; ++z) {
                for(uint32_t x = 0; x < world::CHUNK_WIDTH; ++x) {
                    biomes.push_back(static_cast<uint8_t>(chunk.biome_at(static_cast<int>(x),
                                                                         static_cast<int>(y),
                                                                         static_cast<int>(z))));

                    auto local_sites = chunk.sites_at(static_cast<int>(x),
                                                      static_cast<int>(y),
                                                      static_cast<int>(z));
                    std::transform(std::begin(local_sites), std::end(local_sites), std::back_inserter(resources), [x, z](const site* site) {
                        return networking::resource(x, z, site->type(), site->amount());
                    });
                }
            }
        }

        return networking::world_chunk(chunk.position().x, chunk.position().y, biomes, resources);
    });

    network.send_to(networking::packet::make(chunks_to_send, PACKET_SETUP_CHUNK), connecting_client.socket);
}

void authoritative_game::on_connection(networking::network_manager::socket_handle handle) {
    if(connected_clients.size() >= MAX_CLIENT_COUNT) {
        return;
    }

    client connected_client(handle, static_cast<uint8_t>(connected_clients.size()));

    glm::i32vec2 spawn_position = spawn_chunks[connected_client.id];
    glm::vec3 starting_position(spawn_position.x * world::CHUNK_WIDTH,
                                0.f,
                                spawn_position.y * world::CHUNK_DEPTH);

    std::cout << "client #" << connected_client.id << " spawns at " << spawn_position.x << ", " << spawn_position.y << std::endl;
    connected_client.known_chunks.insert(spawn_position);

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        connected_clients.push_back(connected_client);
    }

    // Send the client's informations
    networking::player_infos infos(connected_client.id);
    network.send_to(networking::packet::make(infos, PACKET_PLAYER_ID), connected_client.socket);

    // Send initial data to the newly connected client
    send_flyweights(handle);

    // Only send initial chunks
    send_map(connected_client);

    // TODO: Improve this

    // TODO: To remove

    //make sure unit doesn't spawn in water or inside ressource
    glm::vec2 availabe_position = find_available_position(world.chunk_at(spawn_chunks[connected_client.id - 1].x, spawn_chunks[connected_client.id - 1].y));
    
    spawn_unit(connected_client.id, starting_position, availabe_position, 100);
}

glm::vec2 authoritative_game::find_available_position(world_chunk* player_chunk)
{
    bool start_position_available = false;
    glm::vec2 player_start_position{0,0};
    for (size_t i = 0; i < world.CHUNK_WIDTH; ++i)
    {
        for (size_t j = 0; j < world.CHUNK_DEPTH; ++j)
        {
            int biome = player_chunk->biome_at(i, 0, j);
            std::vector<site*> sites = player_chunk->sites_at(i, 0, j);
            if (biome != BIOME_WATER && !sites.empty() && sites.front()->type() == SITE_NOTHING)
            {
                player_start_position = { i,j };
                start_position_available = true;
                break;
            }
        }
        if (start_position_available)
        {
            break;
        }
    }
    return player_start_position;
}

void authoritative_game::spawn_unit(uint8_t owner, glm::vec3 position, glm::vec2 target, int flyweight_id) {
    std::cout << "spawning unit " << flyweight_id
              << " for player #" << static_cast<int>(owner)
              << " at " << position.x << ", " << position.y << ", " << position.z
              << std::endl;
    unit_manager& manager = units();
    server_unit_manager& units = static_cast<server_unit_manager&>(manager);
    auto created_unit = units.add_unit_to(owner, make_unit(position, target, flyweight_id));

    std::vector<unit> units_to_spawn;
    units_to_spawn.push_back(*static_cast<unit*>(created_unit.get()));

    auto it = std::find_if(std::begin(connected_clients), std::end(connected_clients), [owner](const client& c) {
        return c.id == owner;
    });
    if(it != std::end(connected_clients)) {
        for(const unit& u : units_to_spawn) {
            it->known_units.emplace(u.get_id());
        }
    }
}

void authoritative_game::broadcast_current_state() {
    std::for_each(std::begin(connected_clients), std::end(connected_clients), [this](const client& c) {
        // Send units known by this client
        std::vector<unit> updated_units;
        std::transform(units().begin_of_units(), units().end_of_units(), std::back_inserter(updated_units),
                       [](const auto &p) {
                           return *p.second;
                       });
        std::vector<unit> known_units;
        known_units.reserve(updated_units.size());
        std::copy_if(std::begin(updated_units), std::end(updated_units), std::back_inserter(known_units), [&c](const unit& u) {
            return c.known_units.find(u.get_id()) != std::end(c.known_units);
        });

        // Send unit update every frame
        if(known_units.size() > 0) {
            network.send_to(networking::packet::make(known_units, PACKET_UPDATE_UNITS), c.socket);
        }
    });

}

void authoritative_game::on_update(frame_duration last_frame) {
    //static frame_duration acc;
    std::chrono::milliseconds last_frame_ms = std::chrono::duration_cast<std::chrono::milliseconds>(last_frame);

    auto received_packets = network.poll_packets();
    for(const std::pair<networking::network_manager::socket_handle, networking::packet>& packet : received_packets) {
        auto packet_socket = packet.first;
        auto it = std::find_if(std::begin(connected_clients), std::end(connected_clients), [packet_socket](const client& c) {
            return c.socket == packet_socket;
        });

        if(it != std::end(connected_clients)) {
            // TODO: Handle packet for this client
            switch(packet.second.head.packet_id) {
                case PACKET_UPDATE_TARGETS: {
                    std::vector<networking::update_target> updates = packet.second.as<std::vector<networking::update_target>>();

                    // Only update units
                    for(const networking::update_target& update : updates) {
                        unit_id id(update.unit_id);

                        // it can move this unit
                        if(id.player_id == it->id) {
                            static_cast<unit*>(units().get(update.unit_id))->set_target_position(update.new_target);
                        }
                    }
                }
                    break;
                default:
                    break;
            }
        }
    }

    auto update_task = push_task(std::make_unique<task::update_units>(units(), world, last_frame_ms.count() / 1000.0f));

    update_task.wait();

    // Wait that units moves to update visibility
    std::vector<async::task_executor::task_future> update_visibility;
    for(client& c : connected_clients) {
        update_visibility.push_back(push_task(std::make_unique<task::update_player_visibility>(c.id, c.map_visibility, units())));
    }

    for(async::task_executor::task_future& future : update_visibility) {
        auto visiblity_ptr = future.get();
        auto visibility_task_ptr = static_cast<task::update_player_visibility*>(visiblity_ptr.get());
        if(visibility_task_ptr) {
            uint8_t player_id = visibility_task_ptr->get_player();
            auto it = std::find_if(std::begin(connected_clients), std::end(connected_clients), [player_id](const client& c) {
               return c.id == player_id;
            });

            if(it != std::end(connected_clients)) {
                it->map_visibility = visibility_task_ptr->visibility();
            }
        }
    }

    // Update known chunks of every clients
    for(client& c : connected_clients) {
        // The players always knows about it's units
        c.known_units.clear();

        std::vector<unit*> players_units;
        units().units_of(c.id, std::back_inserter(players_units));
        std::vector<uint32_t> unit_ids;
        unit_ids.resize(players_units.size());
        std::transform(std::begin(players_units), std::end(players_units), std::begin(unit_ids), [](const unit* u) {
            return u->get_id();
        });
        c.known_units.insert(std::begin(unit_ids), std::end(unit_ids));

        bool has_explored = false;
        for(std::size_t y = 0; y < c.map_visibility.height(); ++y) {
            for(std::size_t x = 0; x < c.map_visibility.width(); ++x) {
                const int chunk_x = x / world::CHUNK_WIDTH;
                const int chunk_z = y / world::CHUNK_DEPTH;

                if(c.map_visibility.at(x, y) != visibility::unexplored) {
                    if(c.known_chunks.find(glm::i32vec2(chunk_x, chunk_z)) == c.known_chunks.end()) {
                        has_explored = true;
                    }

                    c.known_chunks.emplace(chunk_x, chunk_z);
                }

                if(c.map_visibility.at(x, y) == visibility::visible) {
                    std::vector<unit*> units_in_tile;
                    units().units_in(collision::aabb_shape(glm::vec2(x, y), 1.0f), std::back_inserter(units_in_tile));

                    std::vector<uint32_t> units_ids(units_in_tile.size(), 0);
                    std::transform(std::begin(units_in_tile), std::end(units_in_tile), std::begin(units_ids), [](const unit* u) {
                       return u->get_id();
                    });

                    c.known_units.insert(std::begin(units_ids), std::end(units_ids));
                }
            }
        }

        if(has_explored) {
            send_map(c);
        }
    }

    // Broadcast current state every seconds
    if(world_state_sync_clock.elapsed_time<std::chrono::milliseconds>() >= std::chrono::milliseconds(250)) {
        broadcast_current_state();
        world_state_sync_clock.substract(std::chrono::milliseconds(250));
    }
}

void authoritative_game::on_release() {
}