#ifndef MMAP_DEMO_AUTHORITATIVE_GAME_HPP
#define MMAP_DEMO_AUTHORITATIVE_GAME_HPP

#include "../common/game/base_game.hpp"
#include "../common/world/world.hpp"
#include "../common/networking/network_manager.hpp"
#include "../common/networking/packet.hpp"
#include "../common/time/clock.hpp"
#include <unordered_set>

class authoritative_game : public gameplay::base_game {
    struct client {
        static uint8_t next_id;
        networking::network_manager::socket_handle socket;
        uint8_t id;

        // The units this player knows about
        std::unordered_set<uint32_t> known_units;

        // The chunks this player knows about
        std::unordered_set<glm::i32vec2, vec2_hash<glm::i32vec2>> known_chunks;

        explicit client(networking::network_manager::socket_handle socket)
        : socket(socket)
        , id(++next_id) {

        }

        bool operator==(const client& other) const noexcept {
            return id == other.id;
        }
    };

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
