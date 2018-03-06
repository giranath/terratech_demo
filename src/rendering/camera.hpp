#ifndef MMAP_DEMO_CAMERA_HPP
#define MMAP_DEMO_CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class camera {
    static constexpr float X_ANGLE = glm::radians(-35.264f);
    static constexpr float Y_ANGLE = glm::radians(-45.f);

    glm::vec3 pos;
    glm::vec3 target_direction;
    glm::mat4 projection_mat;

public:
    camera(float left, float right, float bottom, float top, float near, float far);

    /// \brief Reset the camera's position
    /// \param pos The new position
    void reset(glm::vec3 pos) noexcept;

    void translate(glm::vec3 translation) noexcept;

    /// \brief Returns the view matrix
    /// \return The view matrix
    glm::mat4 matrix() const noexcept;

    glm::mat4 projection() const noexcept;
    glm::mat4 view() const noexcept;

    glm::vec3 right() const noexcept;
    glm::vec3 up() const noexcept;
    glm::vec3 forward() const noexcept;
    glm::vec3 direction() const noexcept;
};


#endif //MMAP_DEMO_CAMERA_HPP
