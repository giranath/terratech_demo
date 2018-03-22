#ifndef MMAP_DEMO_ALLOCATOR_HPP
#define MMAP_DEMO_ALLOCATOR_HPP

#include "allocator_traits.hpp"

#include <cstdint>
#include <new>

namespace memory {

template<typename T>
struct container_heap_allocator {
    using value_type = T;

    heap_allocator& heap;

    explicit container_heap_allocator(heap_allocator& alloc) : heap{alloc} {}
    //allocator(const allocator&) = default;
    //allocator(const allocator&&) = default;

    value_type* allocate(std::size_t size) {
        auto p = reinterpret_cast<value_type*>(heap.allocate(size * sizeof(value_type)));

        if(!p) throw std::bad_alloc{};

        return p;
    }

    void deallocate(value_type* p, std::size_t size) {
        heap.free(p, sizeof(value_type) * size);
    }
};

}

#endif //MMAP_DEMO_ALLOCATOR_HPP
