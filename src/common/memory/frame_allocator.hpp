#ifndef MMAP_DEMO_FRAME_ALLOCATOR_HPP
#define MMAP_DEMO_FRAME_ALLOCATOR_HPP

#include "allocator_traits.hpp"

#include <cstdint>

namespace memory {

// Doesn't own base_memory
// Clear invalidates all previous allocations
class frame_allocator {
    uint8_t* base_memory;
    uint8_t* next_allocation_ptr;
    std::size_t capacity;

public:
    frame_allocator(uint8_t* base, std::size_t size);

    raw_memory_ptr allocate(std::size_t size);

    void clear();
};

template<>
struct allocator_traits<frame_allocator> {
    static const bool use_fixed_size_allocation = false;
    static const bool can_allocate = true;
    static const bool can_free = false;
    static const bool can_clear = true;
    static const bool can_random_free = false;
};

}

#endif //MMAP_DEMO_FRAME_ALLOCATOR_HPP
