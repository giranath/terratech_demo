#ifndef MMAP_DEMO_AABB_SHAPE_HPP
#define MMAP_DEMO_AABB_SHAPE_HPP

#include <glm/glm.hpp>

namespace collision {

class aabb_shape {
    glm::vec2 center_;
    float width_;
    float height_;
public:
    aabb_shape(glm::vec2 center, float size);
    aabb_shape(glm::vec2 center, float width, float height);

    glm::vec2 center() const noexcept;
    float width() const noexcept;
    float height() const noexcept;
    float left() const noexcept;
    float right() const noexcept;
    float top() const noexcept;
    float bottom() const noexcept;
};

}


#endif //MMAP_DEMO_AABB_SHAPE_HPP
