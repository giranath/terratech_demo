#include "packet.hpp"

#include <numeric>

namespace networking {

header::header(size_type size)
: size(size) {

}

packet::packet(header head)
: head(head)
, bytes(head.size, 0){

}

std::optional<packet> receive_packet_from(const tcp_socket& socket) {
    header::size_type packet_size;
    int recv_size = socket.receive(reinterpret_cast<uint8_t*>(&packet_size), sizeof(packet_size));
    packet_size = SDL_SwapBE64(packet_size);

    // The client has disconnected
    if(recv_size == 0) {
        return {};
    }

    networking::packet received_packet(packet_size);

    header::size_type to_receive_size = packet_size;
    auto current_it = received_packet.bytes.begin();
    const header::size_type PER_ITERATION_MAXLEN = std::numeric_limits<int>::max();

    // If the packet is too big, take multiple iterations to receive the data
    while(to_receive_size > 0) {
        header::size_type len = std::min(PER_ITERATION_MAXLEN, to_receive_size);
        recv_size = socket.receive(&(*current_it), static_cast<int>(len));

        if(recv_size < len) {
            // Client has been disconnected
            return {};
        }
        else {
            to_receive_size -= len;
            current_it = std::next(current_it, len);
        }
    }

    return {std::move(received_packet)};
}

bool send_packet(const tcp_socket& socket, const packet& packet) {
    // Send the header first
    header::size_type packet_size = packet.head.size;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    // convert to big endian
    packet_size = SDL_Swap64(packet_size);
#endif

    socket.send(reinterpret_cast<const uint8_t*>(&packet_size), sizeof(packet_size));

    header::size_type to_send_size = packet.head.size;
    const header::size_type PER_ITERATION_MAXLEN = std::numeric_limits<int>::max();
    auto current_it = packet.bytes.begin();

    while(to_send_size > 0) {
        header::size_type len = std::min(PER_ITERATION_MAXLEN, to_send_size);
        int send_len = socket.send(&(*current_it), static_cast<int>(len));

        if(send_len < len) {
            return false;
        }
        else {
            to_send_size -= len;
            current_it = std::next(current_it, len);
        }
    }

    return true;
}

}