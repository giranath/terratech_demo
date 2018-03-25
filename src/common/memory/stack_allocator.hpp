#ifndef MMAP_DEMO_STACK_ALLOCATOR_HPP
#define MMAP_DEMO_STACK_ALLOCATOR_HPP

#include "allocator_traits.hpp"
#include <cstdint>

namespace memory {

// Doesn't own the memory
// You must free in the inverse order of allocation
class stack_allocator : public base_allocator {
    uint8_t* base_memory;
    uint8_t* next_allocation;
    std::size_t capacity;

public:
    stack_allocator(raw_memory_ptr base_memory, std::size_t capacity);
    raw_memory_ptr allocate(std::size_t size);
    void free(raw_memory_ptr ptr, std::size_t size);

    void clear();
};

template<>
struct allocator_traits<stack_allocator> {
    static const bool use_fixed_size_allocation = false;
    static const bool can_allocate = true;
    static const bool can_free = true;
    static const bool can_clear = true;
    static const bool can_random_free = false;
};

}

#endif //MMAP_DEMO_STACK_ALLOCATOR_HPP
