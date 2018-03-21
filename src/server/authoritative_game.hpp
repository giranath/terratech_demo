#ifndef MMAP_DEMO_AUTHORITATIVE_GAME_HPP
#define MMAP_DEMO_AUTHORITATIVE_GAME_HPP

#include <optional>

#include "../common/game/base_game.hpp"
#include "../common/world/world.hpp"
#include "../common/networking/tcp_socket.hpp"
#include "../common/networking/socket_set.hpp"
#include "../common/networking/header.hpp"

class authoritative_game : public gameplay::base_game {
    using client = networking::tcp_socket;

    infinite_world world;
    networking::socket_set sockets;
    networking::tcp_listener connection_listener;
    std::vector<client> connected_clients;

    void load_flyweights();
    void load_assets();
    void generate_world();
    void setup_listener();

    std::optional<networking::packet> receive_packet_from(const client& c);

    void on_connection();
    void on_client_data(const client& c);
    void on_client_disconnection(const client& c);
    void check_sockets();

public:
    authoritative_game();

    void on_init() override;
    void on_update(frame_duration last_frame) override;
    void on_release() override;
};

#endif //MMAP_DEMO_AUTHORITATIVE_GAME_HPP
