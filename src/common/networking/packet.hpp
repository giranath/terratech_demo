#ifndef MMAP_DEMO_HEADER_HPP
#define MMAP_DEMO_HEADER_HPP

#include "tcp_socket.hpp"

#include <cstdint>
#include <vector>
#include <algorithm>
#include <iterator>
#include <type_traits>

#ifndef __APPLE__
#include <optional>
#else
#include <experimental/optional>
#endif

namespace networking {

struct header {
    using size_type = uint64_t;

    size_type size;

    header(size_type size);
};

struct packet {
    using byte_collection = std::vector<uint8_t>;
    header head;
    byte_collection bytes;

    explicit packet(header head);

    template<class T>
    static packet make(const T& obj) {
        static_assert(std::is_trivially_copyable<T>::value, "the object is not trivial");

        packet p(header(sizeof(obj)));
        p.bytes.reserve(p.head.size);
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&obj);
        std::copy(ptr, ptr + p.head.size, std::back_inserter(p.bytes));

        return p;
    }
    
};

#ifndef __APPLE__
using optional_packet = std::optional<packet>;
#else
using optional_packet = std::experimental::optional<packet>;
#endif

optional_packet receive_packet_from(const tcp_socket& socket);
bool send_packet(const tcp_socket& socket, const packet& packet);

}

#endif //MMAP_DEMO_HEADER_HPP
