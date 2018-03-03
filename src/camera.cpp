#include "camera.hpp"

#include <cmath>

constexpr glm::vec3 spherical_space_vec(float theta, float phi) {
    return {std::cos(phi) * std::sin(theta),
            std::sin(phi),
            std::cos(phi) * std::cos(theta)};
}

camera::camera(float left, float right, float bottom, float top, float near, float far)
: pos{}
, target_direction{spherical_space_vec(Y_ANGLE, X_ANGLE)}
, projection_mat{glm::ortho(left, right, bottom, top, near, far)}{

}

void camera::reset(glm::vec3 new_pos) noexcept {
    pos = new_pos;
}

void camera::translate(glm::vec3 translation) noexcept {
    pos += translation;
}

glm::mat4 camera::matrix() const noexcept {
    return projection() * view();
}

glm::mat4 camera::projection() const noexcept {
    return projection_mat;
}

glm::mat4 camera::view() const noexcept {
    return glm::lookAt(pos, pos + target_direction, {0.f, 1.f, 0.f});
}