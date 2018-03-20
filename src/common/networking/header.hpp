#ifndef MMAP_DEMO_HEADER_HPP
#define MMAP_DEMO_HEADER_HPP

#include <cstdint>
#include <string>

namespace network {

struct header {
    using size_type = uint64_t;

    size_type size;

    header(size_type size)
    : size(size) {

    }
};

struct packet {
    header head;
    std::string raw_data;

    packet(header head, const std::string data)
    : head(head)
    , raw_data(data) {

    }
};

}

#endif //MMAP_DEMO_HEADER_HPP
