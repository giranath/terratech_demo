#include "tcp_socket.hpp"

#include <iostream>
#include <algorithm>

namespace networking {

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