#include "socket_set.hpp"

namespace networking {

socket_set::socket_set(int max)
: sockets(SDLNet_AllocSocketSet(max)) {
}

socket_set::~socket_set() {
    SDLNet_FreeSocketSet(sockets);
}

int socket_set::wait_ms(uint32_t ms) const noexcept {
    return SDLNet_CheckSockets(sockets, ms);
}

void socket_set::add(const tcp_socket& socket) const noexcept {
    SDLNet_TCP_AddSocket(sockets, socket.raw_socket);
}

void socket_set::add(const tcp_listener& listener) const noexcept {
    SDLNet_TCP_AddSocket(sockets, listener.listener_socket);
}

void socket_set::remove(const tcp_socket& socket) const noexcept {
    SDLNet_TCP_DelSocket(sockets, socket.raw_socket);
}

void socket_set::remove(const tcp_listener& socket) const noexcept {
    SDLNet_TCP_DelSocket(sockets, socket.listener_socket);
}

bool socket_set::is_ready(const tcp_socket& socket) const noexcept {
    return SDLNet_SocketReady(socket.raw_socket) != 0;
}

bool socket_set::is_ready(const tcp_listener& socket) const noexcept {
    return SDLNet_SocketReady(socket.listener_socket) != 0;
}

}