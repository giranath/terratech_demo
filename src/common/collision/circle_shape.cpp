#include "circle_shape.hpp"

namespace collision {

circle_shape::circle_shape(glm::vec2 center, float radius)
: radius_(radius)
, center_(center) {

}

glm::vec2 circle_shape::center() const noexcept {
    return center_;
}

float circle_shape::radius() const noexcept {
    return radius_;
}

}