#ifndef MMAP_DEMO_TCP_SOCKET_HPP
#define MMAP_DEMO_TCP_SOCKET_HPP

#ifdef WIN32
#include <SDL_net.h>
#else
#include <SDL2/SDL_net.h>
#endif

#include <cstdint>

namespace networking {

class tcp_socket {
    TCPsocket raw_socket;
public:
    // DO NOT USE
    tcp_socket(TCPsocket s) noexcept;

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

}

#endif //MMAP_DEMO_TCP_SOCKET_HPP
