#ifndef MMAP_DEMO_COLLISION_DETECTOR_HPP
#define MMAP_DEMO_COLLISION_DETECTOR_HPP

#include "aabb_shape.hpp"
#include "circle_shape.hpp"

namespace collision {

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
