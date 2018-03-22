#ifndef MMAP_DEMO_HEAP_ALLOCATOR_HPP
#define MMAP_DEMO_HEAP_ALLOCATOR_HPP

#include "allocator_traits.hpp"
#include <cstdint>

namespace memory {

// Doesn't own it's memory
class heap_allocator {
    struct header {
        header* prev;
        header* next;
        std::size_t allocation_size;
        bool used;
    };

    raw_memory_ptr base_memory;
    std::size_t capacity;
    header* root;

public:
    heap_allocator(raw_memory_ptr base, std::size_t size);

    raw_memory_ptr allocate(std::size_t size);
    void free(raw_memory_ptr memory, std::size_t s);
};

template<>
struct allocator_traits<heap_allocator> {
    static const bool use_fixed_size_allocation = false;
    static const bool can_allocate = true;
    static const bool can_free = true;
    static const bool can_clear = false;
};

}

#endif //MMAP_DEMO_HEAP_ALLOCATOR_HPP
