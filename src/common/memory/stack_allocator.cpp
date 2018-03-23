#include "stack_allocator.hpp"

#include <iterator>

namespace memory {

stack_allocator::stack_allocator(raw_memory_ptr base_memory, std::size_t capacity)
: base_memory(static_cast<uint8_t*>(base_memory))
, next_allocation(static_cast<uint8_t*>(base_memory))
, capacity(capacity) {

}

raw_memory_ptr stack_allocator::allocate(std::size_t size) {
    std::size_t available_space = static_cast<std::size_t>(std::distance(next_allocation, base_memory + capacity));
    if(available_space < size) return nullptr;

    raw_memory_ptr start = next_allocation;
    next_allocation += size;

    return start;
}

void stack_allocator::free(raw_memory_ptr ptr, std::size_t size) {
    if(ptr) {
        next_allocation = static_cast<uint8_t *>(ptr);
    }
}

void stack_allocator::clear() {
    next_allocation = base_memory;
}

}