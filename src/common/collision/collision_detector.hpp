#ifndef MMAP_DEMO_COLLISION_DETECTOR_HPP
#define MMAP_DEMO_COLLISION_DETECTOR_HPP

#include "aabb_shape.hpp"
#include "circle_shape.hpp"

namespace collision {

template<typename T>
struct is_collision_shape {
    static const bool value = false;
};

template<>
struct is_collision_shape<aabb_shape> {
    static const bool value = true;
};

template<>
struct is_collision_shape<glm::vec2> {
    static const bool value = true;
};

template<>
struct is_collision_shape<circle_shape> {
    static const bool value = true;
};

bool detect(const aabb_shape& a, const aabb_shape& b) noexcept;

bool detect(const aabb_shape& a, glm::vec2 b) noexcept;
bool detect(glm::vec2 a, const aabb_shape& b) noexcept;

bool detect(const circle_shape& a, glm::vec2 b) noexcept;
bool detect(glm::vec2 a, const circle_shape& b) noexcept;

bool detect(const circle_shape& a, const circle_shape& b) noexcept;

bool detect(const circle_shape& a, const aabb_shape& b) noexcept;
bool detect(const aabb_shape& a, const circle_shape& b) noexcept;
}

#endif //MMAP_DEMO_COLLISION_DETECTOR_HPP
