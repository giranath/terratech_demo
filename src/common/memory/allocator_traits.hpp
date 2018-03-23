#ifndef MMAP_DEMO_ALLOCATOR_TRAITS_HPP
#define MMAP_DEMO_ALLOCATOR_TRAITS_HPP

namespace memory {

using raw_memory_ptr = void*;

template<typename T>
struct allocator_traits {
    static const bool use_fixed_size_allocation = false;
    static const bool can_allocate = false;
    static const bool can_free = false;
    static const bool can_clear = false;
};

}

#endif //MMAP_DEMO_ALLOCATOR_TRAITS_HPP
