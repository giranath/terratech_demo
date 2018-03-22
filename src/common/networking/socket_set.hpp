#ifndef MMAP_DEMO_SOCKET_SET_HPP
#define MMAP_DEMO_SOCKET_SET_HPP

#ifdef WIN32
#include <SDL_net.h>
#else
#include <SDL2/SDL_net.h>
#endif

#include <chrono>
#include "tcp_socket.hpp"

namespace networking {
class socket_set {
    SDLNet_SocketSet sockets;

    int wait_ms(uint32_t ms) const noexcept;
public:
    socket_set(int max);
    socket_set(const socket_set&) = delete;
    ~socket_set();
    socket_set& operator=(const socket_set&) = delete;

    void add(const tcp_socket& socket) const noexcept;
    void add(const tcp_listener& listener) const noexcept;
    void remove(const tcp_socket& socket) const noexcept;
    void remove(const tcp_listener& socket) const noexcept;

    template<typename Duration>
    int check(Duration duration) const noexcept {
        return wait_ms(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
    }

    bool is_ready(const tcp_socket& socket) const noexcept;
    bool is_ready(const tcp_listener& socket) const noexcept;
};
}


#endif //MMAP_DEMO_SOCKET_SET_HPP
