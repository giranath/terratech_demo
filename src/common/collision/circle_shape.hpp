#ifndef MMAP_DEMO_CIRCLE_SHAPE_HPP
#define MMAP_DEMO_CIRCLE_SHAPE_HPP

#include <glm/glm.hpp>

namespace collision {

class circle_shape {
    float radius_;
    glm::vec2 center_;
public:
    circle_shape(glm::vec2 center, float radius = 1.f);

    glm::vec2 center() const noexcept;
    float radius() const noexcept;
};

}


#endif //MMAP_DEMO_CIRCLE_SHAPE_HPP
