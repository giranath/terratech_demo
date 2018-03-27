#ifndef MMAP_DEMO_SPINLOCK_HPP
#define MMAP_DEMO_SPINLOCK_HPP

#include <atomic>

namespace async {

class spinlock {
    std::atomic_flag lck = ATOMIC_FLAG_INIT;
public:
    spinlock() = default;

    void lock() noexcept;
    bool try_lock() noexcept;
    void unlock() noexcept;
};

}

#endif //MMAP_DEMO_SPINLOCK_HPP
