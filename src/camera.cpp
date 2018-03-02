#include "camera.hpp"


camera::camera(float left, float right, float bottom, float top, float near, float far)
: pos{}
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
    const glm::mat4 translation = glm::translate(glm::mat4{1.f}, -pos);

    return translation * camera::isometric_model();
}

glm::vec3 camera::up() const noexcept {
    return {0.f, 1.f, 0.f};
}

glm::vec3 camera::right() const noexcept {
    return {1.f, 0.f, 0.f};
}

glm::mat4 camera::isometric_model() noexcept {
    const glm::mat4 x_rotation = glm::rotate(glm::mat4{1.f}, 35.264f, {1.f, 0.f, 0.f});
    const glm::mat4 y_rotation = glm::rotate(glm::mat4{1.f}, -45.f, {0.f, 1.f, 0.f});

    return x_rotation * y_rotation;
}