#include "camera.hpp"

#include <iostream>
#include <cmath>
#include <array>
#include <algorithm>
#include <iterator>

namespace rendering {

	glm::vec3 spherical_space_vec(float theta, float phi) {
		return { std::cos(phi) * std::sin(theta),
				std::sin(phi),
				std::cos(phi) * std::cos(theta) };
	}

	camera::camera(float left, float right, float bottom, float top, float near, float far)
		: pos{}, ortho_left{ left }, ortho_right{ right }, ortho_bottom{ bottom }, ortho_top{ top }, ortho_near{ near },
		ortho_far{ far }, target_direction{ spherical_space_vec(Y_ANGLE, X_ANGLE) },
		projection_mat{ glm::ortho(left, right, bottom, top, near, far) } {

	}

	void camera::reset(glm::vec3 new_pos) noexcept {
		pos = new_pos;
	}

	void camera::adjust(float left, float right, float bottom, float top, float near, float far) {
		ortho_left = left;
		ortho_right = right;
		ortho_bottom = bottom;
		ortho_top = top;
		ortho_near = near;
		ortho_far = far;

		projection_mat = glm::ortho(left, right, bottom, top, near, far);
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
		return glm::lookAt(pos, pos + target_direction, up());
	}

	glm::vec3 camera::right() const noexcept {
		return glm::cross(forward(), up());
	}

	glm::vec3 camera::up() const noexcept {
		return glm::vec3{ 0.f, 1.f, 0.f };
	}

	glm::vec3 camera::forward() const noexcept {
		return spherical_space_vec(Y_ANGLE, 0.f);
	}

	glm::vec3 camera::direction() const noexcept {
		return target_direction;
	}

	bounding_cube<float> camera::view_cube() const noexcept {
		const glm::vec3 top_left = pos + glm::vec3{ ortho_left, ortho_top, 0.f };
		const glm::vec3 bottom_left = pos + glm::vec3{ ortho_left, ortho_bottom, 0.f };
		const glm::vec3 top_right = pos + glm::vec3{ ortho_right, ortho_top, 0.f };
		const glm::vec3 bottom_right = pos + glm::vec3{ ortho_right, ortho_bottom, 0.f };

		const glm::vec3 near_top_left = top_left + direction() * ortho_near;
		const glm::vec3 far_top_left = top_left + direction() * ortho_far;
		const glm::vec3 near_top_right = top_right + direction() * ortho_near;
		const glm::vec3 far_top_right = top_right + direction() * ortho_far;
		const glm::vec3 near_bottom_left = bottom_left + direction() * ortho_near;
		const glm::vec3 far_bottom_left = bottom_left + direction() * ortho_far;
		const glm::vec3 near_bottom_right = bottom_right + direction() * ortho_near;
		const glm::vec3 far_bottom_right = bottom_right + direction() * ortho_far;

		std::array<glm::vec3, 8> points{ near_top_left, far_top_left,
										near_top_right, far_top_right,
										near_bottom_left, far_bottom_left,
										near_bottom_right, far_bottom_right };

		auto res_x = std::minmax_element(std::begin(points), std::end(points), [](const glm::vec3 &a, const glm::vec3 &b) {
			return a.x < b.x;
		});
		auto res_y = std::minmax_element(std::begin(points), std::end(points), [](const glm::vec3 &a, const glm::vec3 &b) {
			return a.y < b.y;
		});
		auto res_z = std::minmax_element(std::begin(points), std::end(points), [](const glm::vec3 &a, const glm::vec3 &b) {
			return a.z < b.z;
		});

		return bounding_cube<float>{res_x.first->x, res_y.second->y, res_z.first->z,
			res_x.second->x, res_y.first->y, res_z.second->z};
	}

	glm::vec3 camera::unproject(glm::vec2 screen_coord, float depth) const noexcept {
		glm::vec4 coordinate(screen_coord.x, screen_coord.y, depth, 1.0f);
		glm::mat4 inv = glm::inverse(matrix());

		glm::vec4 temp = inv * coordinate;
		temp /= temp.w;

		return glm::vec3{temp.x, temp.y, temp.z};
	}

	void camera::screen_to_world(glm::vec2 mouse_position, glm::vec3& position, glm::vec3& direction) const noexcept
	{
		position = unproject(mouse_position, 0.f);
		direction = this->direction();
	}

	glm::vec3 camera::line_plane_intersection(glm::vec3 position, glm::vec3 direction, glm::vec3 point_on_plane ,glm::vec3 plane_normal) const noexcept
	{
		float d = glm::dot(point_on_plane -position, plane_normal) / glm::dot(direction, plane_normal);

		return d * direction + position;
	}

    glm::vec3 camera::world_coordinate_of(glm::vec2 normalized_mouse_pos, glm::vec3 point_on_plane, glm::vec3 plane_normal) const noexcept {
        glm::vec3 pos, dir;

        screen_to_world(normalized_mouse_pos, pos, dir);
        return line_plane_intersection(pos, dir, point_on_plane, plane_normal);
    }

	std::pair<glm::vec3, glm::vec3> camera::ray_of(glm::vec2 normalized_screen_coords) const noexcept {
		return std::make_pair(unproject(normalized_screen_coords, -1.f), unproject(normalized_screen_coords, 1.f));
	}

	glm::vec3 camera::position() const noexcept {
		return pos;
	}
}