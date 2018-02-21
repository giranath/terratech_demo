#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

camera::camera(float left, float right, float bottom, float top, float near, float far)
: pos{}
, projection{glm::ortho(left, right, bottom, top, near, far)}{

}

void camera::reset(glm::vec3 new_pos) noexcept {
    pos = new_pos;
}

void camera::translate(glm::vec3 translation) noexcept {
    pos += translation;
}

glm::mat4 camera::matrix() const noexcept {
    const glm::mat4 view_matrix = glm::lookAt(pos, {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f});

    const glm::mat4 x_rotation = glm::rotate(glm::mat4{}, 35.264f, {1.f, 0.f, 0.f});
    const glm::mat4 y_rotation = glm::rotate(glm::mat4{}, -45.f, {0.f, 1.f, 0.f});
    const glm::mat4 translation = glm::translate(glm::mat4{}, -pos);

    return projection * x_rotation * y_rotation * translation;//* ; //* view_matrix;
}