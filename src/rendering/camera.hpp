#ifndef MMAP_DEMO_CAMERA_HPP
#define MMAP_DEMO_CAMERA_HPP

#include "../bounding_box.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#undef near
#undef far

namespace rendering {

class camera {
    static constexpr float X_ANGLE = glm::radians(-35.264f);
    static constexpr float Y_ANGLE = glm::radians(45.f);

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

	/// \param mouse_position Raw Mouse screen position
	/// \param window_height Game screen window height
	/// \param window_width Game screen window width
	/// \param position World position calculated by the
	/// \param direction Direction in the world
	void screen_to_world_raw(const glm::vec2 mouse_position, const int window_widht, const int window_height, glm::vec3& position, glm::vec3& direction) const noexcept;

	/// \param mouse_position Mouse screen position betwen -1 and 1
	/// \param position World position calculated by the
	/// \param direction Direction in the world
	void screen_to_world(const glm::vec2 mouse_position, glm::vec3& position, glm::vec3& direction) const noexcept;

	glm::vec3 LinePlaneIntersection(const glm::vec3 position, const glm::vec3 direction, glm::vec3 plane_normal) const noexcept;

	glm::vec3 position() const noexcept;
};

}


#endif //MMAP_DEMO_CAMERA_HPP
