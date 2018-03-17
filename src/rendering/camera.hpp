#ifndef MMAP_DEMO_CAMERA_HPP
#define MMAP_DEMO_CAMERA_HPP

#include "../bounding_box.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

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

	glm::vec3 unproject(glm::vec2 screen_coord, float depth) const noexcept;

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
	
	/// \param normalized_screen_coords Mouse screen position betwen -1 and 1
	/// \param position World position calculated by the
	/// \param direction Direction in the world
	void screen_to_world(glm::vec2 normalized_screen_coords, glm::vec3& position, glm::vec3& direction) const noexcept;

	glm::vec3 line_plane_intersection(glm::vec3 position, glm::vec3 direction, glm::vec3 point_on_plane, glm::vec3 plane_normal) const noexcept;

	glm::vec3 world_coordinate_of(glm::vec2 normalized_screen_coords, glm::vec3 point_on_plane, glm::vec3 plane_normal) const noexcept;

	std::pair<glm::vec3, glm::vec3> ray_of(glm::vec2 normalized_screen_coords) const noexcept;

	glm::vec3 position() const noexcept;
};

}


#endif //MMAP_DEMO_CAMERA_HPP
