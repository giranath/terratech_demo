#include "malloc_allocator.hpp"

#include <cstdlib>

namespace memory {

raw_memory_ptr malloc_allocator::allocate(std::size_t size) const {
    raw_memory_ptr memory = std::malloc(size);

    if(memory) {
       used_space += size;
    }

    return memory;
}

void malloc_allocator::free(raw_memory_ptr memory, std::size_t size) const {
    used_space -= size;

    std::free(memory);
}

}