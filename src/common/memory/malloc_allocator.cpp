#include "malloc_allocator.hpp"

#include <cstdlib>

namespace memory {

raw_memory_ptr malloc_allocator::allocate(std::size_t size) const {
    return std::malloc(size);
}

void malloc_allocator::free(raw_memory_ptr* memory, std::size_t /*size*/) const {
    std::free(memory);
}

}