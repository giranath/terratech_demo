#include "spinlock.hpp"

namespace async {

void spinlock::lock() noexcept {
    while(lck.test_and_set(std::memory_order_acquire)) {

    }
}

bool spinlock::try_lock() noexcept {
    // Returns true if this thread is the one setting to true
    return !lck.test_and_set(std::memory_order_acquire);
}

void spinlock::unlock() noexcept {
    lck.clear(std::memory_order_release);
}

}