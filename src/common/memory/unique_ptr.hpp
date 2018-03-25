#ifndef MMAP_DEMO_UNIQUE_PTR_HPP
#define MMAP_DEMO_UNIQUE_PTR_HPP

#include "allocator_traits.hpp"
#include <memory>
#include <functional>

namespace memory {

template<typename T, typename Allocator>
class unique_ptr_deleter {
    Allocator& alloc;
public:
    explicit unique_ptr_deleter(Allocator& alloc)
    : alloc(alloc) {

    }

    void operator()(void* ptr) const {
        if(ptr) {
            // Call destructor
            static_cast<T*>(ptr)->~T();
        }

        if constexpr(allocator_traits<Allocator>::can_free) {
            alloc.free(ptr, sizeof(T));
        }
    }
};

template<typename T>
using unique_ptr = std::unique_ptr<T, std::function<void(void*)>>;

template<typename T, typename Allocator, typename... Args>
unique_ptr<T> make_unique(Allocator& alloc, Args&&... arguments) {
    void* raw_memory = alloc.allocate(sizeof(T));

    if(!raw_memory) {
        return unique_ptr<T>{};
    }

    return unique_ptr<T>{new (raw_memory) T(std::forward<Args>(arguments)...),
                         unique_ptr_deleter<T, Allocator>{alloc}};
};

}

#endif //MMAP_DEMO_UNIQUE_PTR_HPP
