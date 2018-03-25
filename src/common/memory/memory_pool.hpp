#ifndef MMAP_DEMO_MEMORY_POOL_HPP
#define MMAP_DEMO_MEMORY_POOL_HPP

#include "allocator_traits.hpp"
#include <cstdint>
#include <cassert>

namespace memory {

// Doesn't own it's memory block
template<typename T>
class memory_pool : public base_allocator {
    struct free_block {
        free_block* next;
    };

    static_assert(sizeof(T) >= sizeof(free_block), "your type is too small");

    raw_memory_ptr base_memory;
    std::size_t capacity;
    free_block* next_free_block;

public:
    memory_pool(raw_memory_ptr memory, std::size_t size)
    : base_memory{memory}
    , capacity{size} {
        clear();
    }

    raw_memory_ptr allocate(std::size_t size = sizeof(T)) {
        assert(size == sizeof(T));

        used_space += size;

        if(next_free_block == nullptr) {
            return nullptr;
        }

        free_block* next_free = next_free_block;
        next_free_block = next_free_block->next;

        return next_free;
    }

    void free(raw_memory_ptr memory, std::size_t size = sizeof(T)) {
        assert(size == sizeof(T));

        if(memory) {
            free_block *free = static_cast<free_block *>(memory);
            free->next = next_free_block;
            next_free_block = free;

            used_space -= size;
        }
    }

    void clear() {
        const std::size_t BLOCK_COUNT = capacity / sizeof(T);

        if(BLOCK_COUNT > 0) {
            T* blocks = static_cast<T *>(base_memory);

            for (int i = 0; i < BLOCK_COUNT - 1; ++i) {
                T* current_block = blocks + i;
                T* next_block = current_block + 1;

                free_block* to_free_block = reinterpret_cast<free_block*>(current_block);
                to_free_block->next = reinterpret_cast<free_block*>(next_block);
            }
            next_free_block = reinterpret_cast<free_block*>(blocks);

            free_block* last_block = reinterpret_cast<free_block*>(blocks[BLOCK_COUNT - 1]);
            if(last_block) {
                last_block->next = nullptr;
            }
        }

        used_space = 0;
    }
};

template<typename T>
struct allocator_traits<memory_pool<T>> {
    static const bool use_fixed_size_allocation = true;
    static const bool can_allocate = true;
    static const bool can_free = true;
    static const bool can_clear = true;
    static const bool can_random_free = true;
};


}

#endif //MMAP_DEMO_MEMORY_POOL_HPP
