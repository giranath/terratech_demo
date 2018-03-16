#include "collision_detector.hpp"

#include <cmath>

namespace collision {

float distance(const glm::vec2& a, const glm::vec2& b) noexcept {
    return (b - a).length();
}

bool detect(const aabb_shape& a, const aabb_shape& b) noexcept {
    return !(a.left() > b.right()   // a trop à droite
          || a.right() < b.left()   // a trop à gauche
          || a.top() < b.bottom()   // a trop bas
          || a.bottom() < b.top()); // a trop haut
}

bool detect(const aabb_shape& a, glm::vec2 b) noexcept {
    return b.x >= a.left() && b.x < a.right() &&
           b.y >= a.bottom() && b.y < a.top();
}

bool detect(glm::vec2 a, const aabb_shape& b) noexcept {
    return detect(b, a);
}

bool detect(const circle_shape& a, glm::vec2 b) noexcept {
    const float dist = (b - a.center()).length();

    return dist <= a.radius();
}

bool detect(glm::vec2 a, const circle_shape& b) noexcept {
    return detect(b, a);
}

bool detect(const circle_shape& a, const circle_shape& b) noexcept {
    const float dist = (b.center() - a.center()).length();

    return dist <= a.radius() + b.radius();
}

bool detect(const circle_shape& a, const aabb_shape& b) noexcept {
    return detect(b, a.center())
        || detect(a, glm::vec2{b.left(), b.top()})
        || detect(a, glm::vec2{b.left(), b.bottom()})
        || detect(a, glm::vec2{b.right(), b.top()})
        || detect(a, glm::vec2{b.right(), b.bottom()});
}

bool detect(const aabb_shape& a, const circle_shape& b) noexcept {
    return detect(b, a);
}

}