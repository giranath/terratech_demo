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

network_manager::socket_handle network_manager::next_handle = 0;

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

void network_manager::network_thread_work_fn(network_manager* manager) {
    manager->thread_work();
}

void network_manager::thread_work() {
    while(is_running) {
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
    std::cout << "new connection" << std::endl;
    std::promise<std::pair<bool, socket_handle>> connection_promise;
    connected_socket connected(next_handle++, std::move(socket), std::move(connection_promise));

#ifndef NCRYPTO
    std::cout << "sending public key ..." << std::endl;
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
                    std::cout << "received public key from server" << std::endl;
                    server_public_key server_key = p->as<server_public_key>();
                    CryptoPP::StringSource ss(server_key.public_key, true);
                    rsa_pub.Load(ss);

                    // Generate random AES key
                    std::default_random_engine engine{static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count())};
                    connection.aes_key = crypto::aes::make_key(engine);

                    client_aes_key client_key;
                    client_key.key.clear();
                    std::copy(std::begin(connection.aes_key), std::end(connection.aes_key), std::back_inserter(client_key.key));

                    std::cout << "sending aes key..." << std::endl;
                    if(send_packet(connection.socket, encrypt(rsa_pub, packet::make(client_key, PACKET_SETUP_ENCRYPTION_KEY)))) {
                        connection.current_state = connected_socket::state::connected;
                        connection.connection_promise.set_value(std::make_pair(true, connection.handle));
                    }
                    else {
                        handle_disconnection(connection);
                    }
                }
                break;
            case connected_socket::state::sending_key:
                if(p->head.packet_id == PACKET_SETUP_ENCRYPTION_KEY) {
                    std::cout << "received aes key" << std::endl;

                    auto decrypted_p = decrypt(rsa_priv, *p);
                    client_aes_key k = decrypted_p.as<client_aes_key>();

                    connection.aes_key.clear();
                    std::copy(std::begin(k.key), std::end(k.key), std::back_inserter(connection.aes_key));

                    // TODO: Mark client as connected
                    connection.current_state = connected_socket::state::connected;
                }
                break;
            case connected_socket::state::connected:
                break;
        }
    }
    else {
        handle_disconnection(connection);
    }
}

void network_manager::handle_disconnection(connected_socket& socket) {

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



std::future<std::pair<bool, network_manager::socket_handle>> network_manager::try_connect(const char* address, uint16_t port) {
    tcp_socket connecting_socket;

    std::promise<std::pair<bool, socket_handle>> connection_promise;
    std::future<std::pair<bool, socket_handle>> connection_future = connection_promise.get_future();

    if(connecting_socket.try_connect(address, port)) {
        int handle = next_handle++;

        active_sockets.add(connecting_socket);
        connected_sockets.emplace_back(handle, std::move(connecting_socket), std::move(connection_promise));
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

}

}