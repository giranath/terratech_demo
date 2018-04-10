#ifndef MMAP_DEMO_AUTHORITATIVE_GAME_HPP
#define MMAP_DEMO_AUTHORITATIVE_GAME_HPP

#include "client.hpp"
#include "../common/game/base_game.hpp"
#include "../common/world/world.hpp"
#include "../common/networking/network_manager.hpp"
#include "../common/networking/packet.hpp"
#include "../common/time/clock.hpp"

class authoritative_game : public gameplay::base_game {
    static const uint8_t MAX_CLIENT_COUNT = 2;
    infinite_world world;
    std::vector<client> connected_clients;
    std::mutex clients_mutex;
    networking::network_manager network;
    game_time::highres_clock world_state_sync_clock;
    glm::i32vec2 spawn_chunks[2];

    void load_flyweights();
    void load_assets();
    void find_spawn_chunks();
    void generate_world();
    void setup_listener();

    glm::vec2 find_available_position(world_chunk* player_chunk);
    void send_flyweights(networking::network_manager::socket_handle client);
    void send_map(const client& connecting_client);
    void on_connection(networking::network_manager::socket_handle handle);
    void spawn_unit(uint8_t owner, glm::vec3 position, glm::vec2 target, int flyweight_id);

    void broadcast_current_state();

public:
    authoritative_game();

    void on_init() override;
    void on_update(frame_duration last_frame) override;
    void on_release() override;
};

#endif //MMAP_DEMO_AUTHORITATIVE_GAME_HPP
