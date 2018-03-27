#include "network_manager.hpp"
#include "networking_constant.hpp"

#include <algorithm>
#include <iterator>
#include <chrono>
#include <random>
#include <crypto++/base64.h>

#if defined(__APPLE__)
#elif defined(WIN32)
#else
#endif

using namespace std::chrono_literals;

namespace networking {

#ifndef NCRYPTO
void to_json(nlohmann::json& json, const server_public_key& key) {
    std::string encoded;
    CryptoPP::StringSource ss(key.public_key, true,
                              new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encoded)));
    json["public key"] = encoded;
}

void from_json(const nlohmann::json& json, server_public_key& key) {
    std::string decoded;
    CryptoPP::StringSource ss(json["public key"], true,
                              new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decoded)));

    key.public_key = std::move(decoded);
}

void to_json(nlohmann::json& json, const client_aes_key& key) {
    std::string encoded;
    CryptoPP::StringSource ss(key.key, true,
                              new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encoded)));
    json["key"] = encoded;
}

void from_json(const nlohmann::json& json, client_aes_key& key) {
    std::string decoded;
    CryptoPP::StringSource ss(json["key"], true,
                              new CryptoPP::Base64Decoder(new CryptoPP::StringSink(decoded)));

    key.key = std::move(decoded);
}
#endif

network_manager::socket_handle network_manager::next_handle = 0;

#ifndef NCRYPTO
packet network_manager::encrypt(crypto::rsa::public_key key, const packet& p) {
    std::vector<uint8_t> encrypted_body;
    crypto::rsa::encrypt(key, std::begin(p.bytes), std::end(p.bytes), std::back_inserter(encrypted_body));

    header head(encrypted_body.size(), p.head.packet_id);
    return packet(head, std::move(encrypted_body));
}

packet network_manager::decrypt(crypto::rsa::private_key key, const packet& p) {
    std::vector<uint8_t> decrypted_body;
    crypto::rsa::decrypt(key, std::begin(p.bytes), std::end(p.bytes), std::back_inserter(decrypted_body));

    header head(decrypted_body.size(), p.head.packet_id);
    return packet(head, std::move(decrypted_body));
}

packet network_manager::encrypt(crypto::aes::key key, const packet& p) {
    std::vector<uint8_t> encrypted_body;
    crypto::aes::encrypt(key, std::begin(p.bytes), std::end(p.bytes), std::back_inserter(encrypted_body));

    header head(encrypted_body.size(), p.head.packet_id);
    return packet(head, std::move(encrypted_body));
}

packet network_manager::decrypt(crypto::aes::key key, const packet& p) {
    std::vector<uint8_t> decrypted_body;
    crypto::aes::decrypt(key, std::begin(p.bytes), std::end(p.bytes), std::back_inserter(decrypted_body));

    header head(decrypted_body.size(), p.head.packet_id);
    return packet(head, std::move(decrypted_body));
}
#endif

void network_manager::network_thread_work_fn(network_manager* manager) {
    manager->thread_work();
}

void network_manager::thread_work() {
    while(is_running) {
        std::vector<std::pair<socket_handle, packet>> packets_to_send;
        {
            std::lock_guard<async::spinlock> lock(waiting_spin_lock);
            packets_to_send.swap(waiting_queue);
        }

        // Send packets
        std::for_each(std::begin(packets_to_send), std::end(packets_to_send), [this](const std::pair<socket_handle, packet>& p) {
            auto socket_it = std::find_if(std::begin(connected_sockets), std::end(connected_sockets), [=](connected_socket& connection) {
                return connection.handle == p.first && connection.current_state == connected_socket::state::connected;
            });

            if(socket_it != connected_sockets.end()) {
#ifndef NCRYPTO
                auto encrypted_packet = encrypt(socket_it->aes_key, p.second);
#else
                const packet& encrypted_packet = p.second;
#endif
                if(!send_packet(socket_it->socket, encrypted_packet)) {
                    // Failed to send data
                    handle_disconnection(*socket_it);
                }
            }
        });

        // Check if a socket has received data
        if(active_sockets.check(30ms) > 0) {
            if(active_sockets.is_ready(connection_listener)) {
                handle_connection(connection_listener.accept());
            }
            else {
                std::for_each(std::begin(connected_sockets), std::end(connected_sockets), [this](connected_socket& connection) {
                    if(active_sockets.is_ready(connection.socket)) {
                        handle_connected_socket(connection);
                    }
                });
            }
        }
    }
}

void network_manager::handle_connection(tcp_socket socket) {
    std::promise<std::pair<bool, socket_handle>> connection_promise;
    connected_socket connected(next_handle++, std::move(socket), std::move(connection_promise));

#ifndef NCRYPTO
    server_public_key k;
    CryptoPP::StringSink ss(k.public_key);
    rsa_pub.Save(ss);

    auto p = packet::make(k, PACKET_SERVER_PUBLIC_KEY);
    // Send public key
    if(send_packet(connected.socket, packet::make(k, PACKET_SERVER_PUBLIC_KEY))) {
        connected.current_state = connected_socket::state::sending_key;
    }
    else {
        // The client has disconnected
        return;
    }
#else
    // There is no encryption
    connected.current_state = connected_socket::state::connected;
    on_connection.call(connected.handle);
#endif

    active_sockets.add(connected.socket);
    connected_sockets.push_back(std::move(connected));
}

void network_manager::handle_connected_socket(connected_socket& connection) {
    auto p = receive_packet_from(connection.socket);
    if(p) {
        switch (connection.current_state) {
            case connected_socket::state::waiting_public_key:
                if(p->head.packet_id == PACKET_SERVER_PUBLIC_KEY) {
#ifndef NCRYPTO
                    server_public_key server_key = p->as<server_public_key>();
                    CryptoPP::StringSource ss(server_key.public_key, true);
                    rsa_pub.Load(ss);

                    // Generate random AES key
                    std::default_random_engine engine{static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count())};
                    connection.aes_key = crypto::aes::make_key(engine);

                    client_aes_key client_key;
                    client_key.key.clear();
                    std::copy(std::begin(connection.aes_key), std::end(connection.aes_key), std::back_inserter(client_key.key));

                    if(send_packet(connection.socket, encrypt(rsa_pub, packet::make(client_key, PACKET_SETUP_ENCRYPTION_KEY)))) {
                        connection.current_state = connected_socket::state::connected;
                        connection.connection_promise.set_value(std::make_pair(true, connection.handle));
                    }
                    else {
                        handle_disconnection(connection);
                    }
#else
                    throw std::runtime_error("encryption is disabled");
#endif
                }
                break;
            case connected_socket::state::sending_key:
                if(p->head.packet_id == PACKET_SETUP_ENCRYPTION_KEY) {
#ifndef NCRYPTO
                    auto decrypted_p = decrypt(rsa_priv, *p);
                    client_aes_key k = decrypted_p.as<client_aes_key>();

                    connection.aes_key.clear();
                    std::copy(std::begin(k.key), std::end(k.key), std::back_inserter(connection.aes_key));

                    connection.current_state = connected_socket::state::connected;
                    on_connection.call(connection.handle);
#else
                    throw std::runtime_error("encryption is disabled");
#endif
                }
                break;
            case connected_socket::state::connected:
            {
#ifndef NCRYPTO
                auto decrypted_packet = decrypt(connection.aes_key, *p);
#else
                const packet& decrypted_packet = *p;
#endif
                {
                    std::lock_guard<async::spinlock> lock(received_lock);

                    auto it = std::find_if(std::begin(waiting_recv_requests), std::end(waiting_recv_requests),
                                           [&connection, &decrypted_packet](const waiting_receive_request &req) {
                                               return req.src == connection.handle
                                                      && req.type == decrypted_packet.head.packet_id;
                                           });
                    if(it != std::end(waiting_recv_requests)) {
                        it->promise.set_value(std::make_pair(true, decrypted_packet));
                        waiting_recv_requests.erase(it);
                    }
                    else {
#ifndef NCRYPTO
                        received_requests.emplace_back(connection.handle, std::move(decrypted_packet));
#else
                        received_requests.emplace_back(connection.handle, decrypted_packet);
#endif
                    }
                }
            }
                break;
        }
    }
    else {
        handle_disconnection(connection);
    }
}

void network_manager::handle_disconnection(connected_socket& connection) {
    active_sockets.remove(connection.socket);
    std::swap(connection, connected_sockets.back());
    connected_sockets.pop_back();

    if(connection.current_state == connected_socket::state::connected) {
        on_disconnection.call(connection.handle);
    }
}

network_manager::network_manager(int max_socket_count)
: active_sockets(max_socket_count)
, is_running(true) {
    network_thread = std::thread(network_thread_work_fn, this);
}

network_manager::~network_manager() {
    is_running = false;
    network_thread.join();
}

bool network_manager::try_bind(uint16_t port) {
    if(connection_listener.try_bind(port)) {
        active_sockets.add(connection_listener);
        return true;
    }

    return false;
}

bool network_manager::is_bound() const noexcept {
    return connection_listener.is_bound();
}

std::future<std::pair<bool, network_manager::socket_handle>> network_manager::try_connect(const char* address, uint16_t port) {
    tcp_socket connecting_socket;

    std::promise<std::pair<bool, socket_handle>> connection_promise;
    std::future<std::pair<bool, socket_handle>> connection_future = connection_promise.get_future();

    if(connecting_socket.try_connect(address, port)) {
        int handle = next_handle++;

        active_sockets.add(connecting_socket);
#ifndef NCRYPTO
        connected_sockets.emplace_back(handle, std::move(connecting_socket), std::move(connection_promise));
#else
        connected_socket connection(handle, std::move(connecting_socket), std::move(connection_promise));
        connection.current_state = connected_socket::state::connected;
        connected_sockets.push_back(std::move(connection));
        connected_sockets.back().connection_promise.set_value(std::make_pair(true, connected_sockets.back().handle));
#endif
    }
    else {
        connection_promise.set_value(std::make_pair(false, 0));
    }

    return connection_future;
}

void network_manager::load_rsa_keys(const char* private_key, const char *public_key) {
#ifndef NCRYPTO
    rsa_priv = crypto::rsa::load_key<crypto::rsa::private_key >(private_key);
    rsa_pub = crypto::rsa::load_key<crypto::rsa::public_key>(public_key);
#endif
}

void network_manager::send_to(const packet& p, socket_handle dest) {
    std::lock_guard<async::spinlock> lock(waiting_spin_lock);
    waiting_queue.emplace_back(dest, p);
}

void network_manager::broadcast(const packet& p) {
    std::lock_guard<async::spinlock> lock(waiting_spin_lock);
    std::for_each(std::begin(connected_sockets), std::end(connected_sockets), [this, &p](const connected_socket& connection) {
        waiting_queue.emplace_back(connection.handle, p);
    });
}

std::future<std::pair<bool, packet>> network_manager::receive_from(int packet_type, socket_handle src) {
    std::lock_guard<async::spinlock> lock(received_lock);
    auto it = std::find_if(std::begin(received_requests), std::end(received_requests), [=](receive_request &req) {
        return req.src == src && req.content.head.packet_id == packet_type;
    });

    if (it != std::end(received_requests)) {
        std::promise<std::pair<bool, packet>> promise;
        auto future = promise.get_future();

        promise.set_value(std::make_pair(true, it->content));
        received_requests.erase(it);

        return future;
    }

    waiting_receive_request req(src, packet_type);
    auto future = req.promise.get_future();
    waiting_recv_requests.push_back(std::move(req));
    return future;
}

}