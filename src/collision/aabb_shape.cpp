#include "aabb_shape.hpp"

namespace collision {

aabb_shape::aabb_shape(glm::vec2 center, float size)
: center_(center)
, width_(size)
, height_(size) {

}

aabb_shape::aabb_shape(glm::vec2 center, float width, float height)
: center_(center)
, width_(width)
, height_(height) {

}

glm::vec2 aabb_shape::center() const noexcept {
    return center_;
}

float aabb_shape::width() const noexcept {
    return width_;
}

float aabb_shape::height() const noexcept {
    return height_;
}

float aabb_shape::left() const noexcept {
    return center().x - width() / 2.f;
}

float aabb_shape::right() const noexcept {
    return center().x + width() / 2.f;
}

float aabb_shape::top() const noexcept {
    return center().y + height() / 2.f;
}

float aabb_shape::bottom() const noexcept {
    return center().y - height() / 2.f;
}

}