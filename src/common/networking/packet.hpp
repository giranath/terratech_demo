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

#include <json/json.hpp>

namespace networking {

struct header {
    using size_type = uint64_t;
    using packet_id_type = uint8_t;
    size_type size;
    packet_id_type packet_id;
    header(size_type size, packet_id_type packet_type);
};

struct packet {
    using byte_collection = std::vector<uint8_t>;
    header head;
    byte_collection bytes;

    packet();

    explicit packet(header head);
    packet(header, byte_collection&& bytes);

    template<class T>
    static packet make(const T& obj, const header::packet_id_type& packet_id) {
        nlohmann::json json = obj;

        std::string json_str = json.dump();
        packet p(header(json_str.size(), packet_id));
        std::transform(std::begin(json_str), std::end(json_str), std::begin(p.bytes), [](char letter) {
            return static_cast<uint8_t>(letter);
        });

        return p;
    }

    template <class T>
    T as() const {
        nlohmann::json json = nlohmann::json::parse(std::begin(bytes), std::end(bytes));
        return json.get<T>();
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
