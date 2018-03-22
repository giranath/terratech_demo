#ifndef MMAP_DEMO_MALLOC_ALLOCATOR_HPP
#define MMAP_DEMO_MALLOC_ALLOCATOR_HPP

#include "allocator_traits.hpp"

#include <cstddef>

namespace memory {

class malloc_allocator {
public:
    raw_memory_ptr allocate(std::size_t size) const;

    void free(raw_memory_ptr memory, std::size_t size) const;
};

template<>
struct allocator_traits<malloc_allocator> {
    static const bool use_fixed_size_allocation = false;
    static const bool can_allocate = true;
    static const bool can_free = true;
    static const bool can_clear = false;
};

}

#endif //MMAP_DEMO_HEAP_ALLOCATOR_HPP
