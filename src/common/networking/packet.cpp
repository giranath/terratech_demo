#include "packet.hpp"

#include <numeric>
#include <iostream>
namespace networking {

header::header(size_type size, packet_id_type packet_id)
: size(size)
, packet_id{packet_id} {

}

packet::packet()
: head(0, 0)
, bytes() {

}

packet::packet(header head)
: head(head)
, bytes(head.size, 0){

}

packet::packet(header h, byte_collection&& bytes)
: head(h)
, bytes(std::move(bytes)) {

}

optional_packet receive_packet_from(const tcp_socket& socket) {
    header::size_type packet_size;
    int recv_size = socket.receive(reinterpret_cast<uint8_t*>(&packet_size), sizeof(packet_size));
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    packet_size = SDL_Swap64(packet_size);
#endif

    // The client has disconnected
    if(static_cast<header::size_type>(recv_size) < sizeof(header::size_type)) {
        return {};
    }

    header::packet_id_type packet_id;
    recv_size = socket.receive(reinterpret_cast<uint8_t*>(&packet_id), sizeof(packet_id));

    // The client has disconnected
    if (static_cast<header::packet_id_type>(recv_size) < sizeof(header::packet_id)) {
        return {};
    }

    networking::packet received_packet(header{ packet_size, packet_id });

    header::size_type to_receive_size = packet_size;
    auto current_it = received_packet.bytes.begin();
    const header::size_type PER_ITERATION_MAXLEN = std::numeric_limits<int>::max();

    // If the packet is too big, take multiple iterations to receive the data
    while(to_receive_size > 0) {
        header::size_type len = std::min(PER_ITERATION_MAXLEN, to_receive_size);
        recv_size = socket.receive(&(*current_it), static_cast<int>(len));

        if(recv_size <= 0) {
            // Client has been disconnected
            return {};
        }
        else {
            to_receive_size -= recv_size;
            current_it = std::next(current_it, recv_size);
        }
    }

    assert(received_packet.head.size == received_packet.bytes.size());

    return {std::move(received_packet)};
}

bool send_packet(const tcp_socket& socket, const packet& packet) {
    // Send the header first
    header::size_type packet_size = packet.head.size;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    // convert to big endian
    packet_size = SDL_Swap64(packet_size);
#endif

    if(socket.send(reinterpret_cast<const uint8_t*>(&packet_size), sizeof(packet_size)) <= 0) {
        return false;
    }

    if(socket.send(reinterpret_cast<const uint8_t*>(&packet.head.packet_id), sizeof(packet.head.packet_id)) <= 0) {
        return false;
    }

    header::size_type to_send_size = packet.head.size;
    const header::size_type PER_ITERATION_MAXLEN = std::numeric_limits<int>::max();
    auto current_it = packet.bytes.begin();

    while(to_send_size > 0) {
        header::size_type len = std::min(PER_ITERATION_MAXLEN, to_send_size);
        int send_len = socket.send(&(*current_it), static_cast<int>(len));

        if(send_len <= 0) {
            return false;
        }
        else {
            to_send_size -= send_len;
            current_it = std::next(current_it, send_len);
        }
    }

    return true;
}

}