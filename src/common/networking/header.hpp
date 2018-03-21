#ifndef MMAP_DEMO_HEADER_HPP
#define MMAP_DEMO_HEADER_HPP

#include <cstdint>
#include <vector>

namespace networking {

struct header {
    using size_type = uint64_t;

    size_type size;

    header(size_type size)
    : size(size) {

    }
};

struct packet {
    header head;
    std::vector<uint8_t> bytes;

    explicit packet(header head)
    : head(head)
    , bytes(head.size, 0){

    }
};

}

#endif //MMAP_DEMO_HEADER_HPP
