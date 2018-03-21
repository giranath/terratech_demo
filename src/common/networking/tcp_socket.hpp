#ifndef MMAP_DEMO_TCP_SOCKET_HPP
#define MMAP_DEMO_TCP_SOCKET_HPP

#ifdef WIN32
#include <SDL_net.h>
#else
#include <SDL2/SDL_net.h>
#endif

#include <cstdint>

namespace networking {

class socket_set;
class tcp_listener;

class tcp_socket {
    friend tcp_listener;
    friend socket_set;
    TCPsocket raw_socket;

    // DO NOT USE
    tcp_socket(TCPsocket s) noexcept;
public:
    tcp_socket();
    tcp_socket(const tcp_socket&) = delete;
    tcp_socket(tcp_socket&& other) noexcept;
    ~tcp_socket() noexcept;

    tcp_socket& operator=(const tcp_socket&) = delete;
    tcp_socket& operator=(tcp_socket&& other) noexcept;

    bool try_connect(const char* address, uint16_t port) noexcept;
    bool is_connected() const noexcept;

    int receive(uint8_t* data, int data_len) noexcept;
    int send(const uint8_t* data, int data_len) noexcept;

    operator TCPsocket() const noexcept;
};

class tcp_listener {
    friend socket_set;
    TCPsocket listener_socket;
public:
    tcp_listener();
    tcp_listener(const tcp_listener&) = delete;
    tcp_listener(tcp_listener&& other) noexcept;
    ~tcp_listener();
    tcp_listener& operator=(tcp_listener&& other) noexcept;

    tcp_listener& operator=(const tcp_listener&) = delete;

    tcp_socket accept() const noexcept;
    bool try_bind(uint16_t port) noexcept;
    bool is_bound() const noexcept;

    operator TCPsocket() const noexcept;
};

}

#endif //MMAP_DEMO_TCP_SOCKET_HPP
