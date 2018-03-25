#ifndef MMAP_DEMO_ALLOCATOR_TRAITS_HPP
#define MMAP_DEMO_ALLOCATOR_TRAITS_HPP

#include <cstddef>

namespace memory {

using raw_memory_ptr = void*;

class base_allocator {
protected:
    mutable std::size_t used_space{};
public:
    base_allocator() = default;

    std::size_t allocated_space() const noexcept {
        return used_space;
    }
};

template<typename T>
struct allocator_traits {
    static const bool use_fixed_size_allocation = false;
    static const bool can_allocate = false;
    static const bool can_free = false;
    static const bool can_clear = false;
    static const bool can_random_free = false;
};

}

#endif //MMAP_DEMO_ALLOCATOR_TRAITS_HPP
