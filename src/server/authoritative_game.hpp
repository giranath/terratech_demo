#ifndef MMAP_DEMO_AUTHORITATIVE_GAME_HPP
#define MMAP_DEMO_AUTHORITATIVE_GAME_HPP

#include "../common/game/base_game.hpp"
#include "../common/world/world.hpp"
#include "../common/networking/network_manager.hpp"
#include "../common/networking/packet.hpp"

class authoritative_game : public gameplay::base_game {
    struct client {
        static uint8_t next_id;
        networking::network_manager::socket_handle socket;
        uint8_t id;

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

    void load_flyweights();
    void load_assets();
    void generate_world();
    void setup_listener();

    void on_connection(networking::network_manager::socket_handle handle);
    void spawn_unit(uint8_t owner, glm::vec3 position, glm::vec2 target, int flyweight_id);

public:
    authoritative_game();

    void on_init() override;
    void on_update(frame_duration last_frame) override;
    void on_release() override;
};

#endif //MMAP_DEMO_AUTHORITATIVE_GAME_HPP
