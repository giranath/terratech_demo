#ifndef MMAP_DEMO_VEC_HASH_HPP
#define MMAP_DEMO_VEC_HASH_HPP

#include <glm/glm.hpp>
#include <functional>

namespace util {

template<typename T>
struct vec3_hash {
    std::size_t operator()(const T& vec) const {
        using val_type = typename T::value_type;
        return ((std::hash<val_type>()(vec.x)
                 ^ (std::hash<val_type>()(vec.y) << 1)) >> 1)
               ^ (std::hash<val_type>()(vec.z) << 1);
    }
};

template<typename T>
struct vec2_hash {
    std::size_t operator()(const T& vec) const {
        using val_type = typename T::value_type;
        return ((std::hash<val_type>()(vec.x)
                 ^ (std::hash<val_type>()(vec.y) << 1)) >> 1);
    }
};

}

#endif //MMAP_DEMO_VEC_HASH_HPP
