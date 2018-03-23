#ifndef MMAP_DEMO_UTILS_HPP
#define MMAP_DEMO_UTILS_HPP

#include <cstdint>

namespace memory {

constexpr std::size_t gigabits(std::size_t giga) {
    return giga * 1024 * 1024 * 1024;
}

}

#endif //MMAP_DEMO_UTILS_HPP
