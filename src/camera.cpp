#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void camera::reset(glm::vec3 new_pos) noexcept {
    pos = new_pos;
}

glm::mat4 camera::matrix() const noexcept {
    return glm::translate(projection, pos);
}