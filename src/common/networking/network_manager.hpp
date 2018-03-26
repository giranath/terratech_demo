#ifndef MMAP_DEMO_NETWORK_MANAGER_HPP
#define MMAP_DEMO_NETWORK_MANAGER_HPP

#include "socket_set.hpp"
#include "tcp_socket.hpp"
#include "packet.hpp"
#include "../crypto/rsa.hpp"
#include "../crypto/aes.hpp"
#include "../async/event.hpp"
#include "../async/spinlock.hpp"

#include <thread>
#include <atomic>
#include <vector>
#include <optional>
#include <future>

namespace networking {

struct server_public_key {
    std::string public_key;
};

struct client_aes_key {
    std::string key;
};

void to_json(nlohmann::json& json, const server_public_key& key);
void from_json(const nlohmann::json& json, server_public_key& key);
void to_json(nlohmann::json& json, const client_aes_key& key);
void from_json(const nlohmann::json& json, client_aes_key& key);

// The network manager manages the details of the communication between
// connected clients
class network_manager {
public:
    using socket_handle = int;
    using connection_event = async::event<socket_handle>;
    connection_event on_connection;

private:
    std::thread network_thread;
    socket_set active_sockets;
    std::atomic<bool> is_running;
    tcp_listener connection_listener;

    struct connected_socket {
        enum class state {
            waiting_public_key,
            sending_key,
            connected
        };

        tcp_socket socket;
        socket_handle handle;
        state current_state;
        std::promise<std::pair<bool, socket_handle>> connection_promise;
#ifndef NCRYPTO
        crypto::aes::key aes_key;
#endif

        connected_socket(socket_handle hdl, tcp_socket&& socket, std::promise<std::pair<bool, socket_handle>>&& conn_prom)
        : socket(std::move(socket))
        , handle(hdl)
        , current_state(state::waiting_public_key)
        , connection_promise(std::move(conn_prom))
#ifndef NCRYPTO
        , aes_key()
#endif
        {

        }
    };

    std::vector<connected_socket> connected_sockets;
#ifndef NCRYPTO
    crypto::rsa::public_key rsa_pub;
    crypto::rsa::private_key rsa_priv;
#endif

    std::vector<std::pair<socket_handle, packet>> waiting_queue;
    async::spinlock waiting_spin_lock;

    static socket_handle next_handle;

    static void network_thread_work_fn(network_manager* manager);
    void thread_work();

    void handle_connection(tcp_socket socket);
    void handle_connected_socket(connected_socket& socket);
    void handle_disconnection(connected_socket& socket);

    static packet encrypt(crypto::rsa::public_key key, const packet& p);
    static packet decrypt(crypto::rsa::private_key key, const packet& p);
    static packet encrypt(crypto::aes::key key, const packet& p);
    static packet decrypt(crypto::aes::key key, const packet& p);

public:
    network_manager(int max_socket_count);
    ~network_manager();

    bool try_bind(uint16_t port);
    bool is_bound() const noexcept;

    void load_rsa_keys(const char* private_key, const char* public_key);

    std::future<std::pair<bool, socket_handle>> try_connect(const char* address, uint16_t port);

    void send_to(const packet& p, socket_handle dest);
};

}


#endif //MMAP_DEMO_NETWORK_MANAGER_HPP
