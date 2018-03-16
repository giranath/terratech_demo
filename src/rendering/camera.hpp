#ifndef MMAP_DEMO_CAMERA_HPP
#define MMAP_DEMO_CAMERA_HPP

#include "../bounding_box.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#undef near
#undef far

class camera {
    static constexpr float X_ANGLE = glm::radians(-35.264f);
    static constexpr float Y_ANGLE = glm::radians(-45.f);

    glm::vec3 pos;
    float ortho_left, ortho_right, ortho_bottom, ortho_top, ortho_near, ortho_far;
    glm::vec3 target_direction;
    glm::mat4 projection_mat;

public:
    camera(float left, float right, float bottom, float top, float near, float far);

    /// \brief Reset the camera's position
    /// \param pos The new position
    void reset(glm::vec3 pos) noexcept;

    void translate(glm::vec3 translation) noexcept;

    void adjust(float left, float right, float bottom, float top, float near, float far);

    /// \brief Returns the view matrix
    /// \return The view matrix
    glm::mat4 matrix() const noexcept;

    glm::mat4 projection() const noexcept;
    glm::mat4 view() const noexcept;

    glm::vec3 right() const noexcept;
    glm::vec3 up() const noexcept;
    glm::vec3 forward() const noexcept;
    glm::vec3 direction() const noexcept;

    bounding_cube<float> view_cube() const noexcept;

	void screen_to_world_raw(const glm::vec2 mouse_position, const int window_height, const int window_widht, glm::vec3& position, glm::vec3& direction) const noexcept;
	void screen_to_world(const glm::vec2 mouse_position, glm::vec3& position, glm::vec3& direction) const noexcept;
};


#endif //MMAP_DEMO_CAMERA_HPP
