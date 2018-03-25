#include "frame_allocator.hpp"

#include <algorithm>

namespace memory {

frame_allocator::frame_allocator(uint8_t* base, std::size_t size)
: base_memory(base)
, next_allocation_ptr(base)
, capacity(size) {

}

raw_memory_ptr frame_allocator::allocate(std::size_t size) {
    const std::size_t available_space = std::distance(next_allocation_ptr, base_memory + size);

    if(available_space < size) {
        return nullptr;
    }

    raw_memory_ptr allocated_mem = next_allocation_ptr;
    next_allocation_ptr += size;
    used_space += size;

    return allocated_mem;
}

void frame_allocator::clear() {
    next_allocation_ptr = base_memory;
    used_space = 0;
}

}