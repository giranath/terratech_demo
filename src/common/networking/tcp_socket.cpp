#include "tcp_socket.hpp"

#include <iostream>
#include <algorithm>

namespace networking {

tcp_listener::tcp_listener()
: listener_socket(nullptr) {

}

tcp_listener::tcp_listener(tcp_listener&& other) noexcept
: listener_socket(other.listener_socket) {
    other.listener_socket = nullptr;
}

tcp_listener::~tcp_listener() {
    SDLNet_TCP_Close(listener_socket);
}

tcp_listener& tcp_listener::operator=(tcp_listener &&other) noexcept {
    std::swap(listener_socket, other.listener_socket);

    return *this;
}

tcp_listener::operator TCPsocket() const noexcept {
    return listener_socket;
}

tcp_socket tcp_listener::accept() const noexcept {
    TCPsocket client_socket = SDLNet_TCP_Accept(listener_socket);

    return tcp_socket(client_socket);
}

bool tcp_listener::try_bind(uint16_t port) noexcept {
    IPaddress ip;
    if(SDLNet_ResolveHost(&ip, nullptr, port) == -1) {
        std::cerr << "failed to bind " << port << std::endl;
        return false;
    }

    listener_socket = SDLNet_TCP_Open(&ip);
    return true;
}

tcp_socket::tcp_socket(TCPsocket s) noexcept
: raw_socket(s) {

}

tcp_socket::tcp_socket()
: raw_socket(nullptr) {

}

tcp_socket::tcp_socket(tcp_socket&& other) noexcept
: raw_socket(other.raw_socket){
    other.raw_socket = nullptr;
}

tcp_socket::~tcp_socket() noexcept {
    SDLNet_TCP_Close(raw_socket);
}

tcp_socket& tcp_socket::operator=(tcp_socket&& other) noexcept {
    std::swap(raw_socket, other.raw_socket);

    return *this;
}

bool tcp_socket::try_connect(const char* address, uint16_t port) noexcept {
    IPaddress ip;
    if(SDLNet_ResolveHost(&ip, address, port) == -1) {
        std::cerr << "failed to resolve " << address << ":" << port << std::endl;
        return false;
    }

    raw_socket = SDLNet_TCP_Open(&ip);

    return raw_socket != nullptr;
}

bool tcp_socket::is_connected() const noexcept {
    return raw_socket != nullptr;
}

int tcp_socket::receive(uint8_t* data, int data_len) noexcept {
    int v = SDLNet_TCP_Recv(raw_socket, data, data_len);

    if(v < 0) {
        SDLNet_TCP_Close(raw_socket);
        raw_socket = nullptr;
    }

    return v;
}

int tcp_socket::send(const uint8_t *data, int data_len) noexcept {
    int v = SDLNet_TCP_Send(raw_socket, data, data_len);

    if(v < 0) {
        SDLNet_TCP_Close(raw_socket);
        raw_socket = nullptr;
    }

    return v;
}

tcp_socket::operator TCPsocket() const noexcept {
    return raw_socket;
}

}