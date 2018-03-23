#ifndef MMAP_DEMO_ALLOCATOR_HPP
#define MMAP_DEMO_ALLOCATOR_HPP

#include "allocator_traits.hpp"
#include "heap_allocator.hpp"

#include <cstdint>
#include <new>

namespace memory {

template<typename T>
struct container_heap_allocator {
    using value_type = T;

    heap_allocator& heap;

    explicit container_heap_allocator(heap_allocator& alloc)
    : heap{alloc} {

    }

    container_heap_allocator(const container_heap_allocator&) = default;
    container_heap_allocator(container_heap_allocator&&) = default;

    template<typename U>
    container_heap_allocator(const container_heap_allocator<U>& other)
    : heap(other.heap) {

    }

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
