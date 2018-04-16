#ifndef MMAP_DEMO_MESH_HPP
#define MMAP_DEMO_MESH_HPP

#include "../opengl/opengl.hpp"
#include "../src/common/memory/static_vector.hpp"
#include "../../common/bounding_box.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace rendering {

	class mesh;
	class mesh_builder {

	public:
		mesh_builder() = default;
		virtual ~mesh_builder() = default;
		virtual void add_vertex(glm::vec3 vertex, glm::vec2 uv, glm::vec3 color = { 1.f, 1.f, 1.f }) = 0;

		mesh build() const noexcept;
		void rebuild(mesh& m) const noexcept;
		virtual size_t count() const = 0;
		virtual const glm::vec3* get_vertices() const = 0;
		virtual const glm::vec3* get_colors() const = 0;
		virtual const glm::vec2* get_uvs() const = 0;
	};

	template <size_t MAX_CAPACITY>
	class static_mesh_builder : public mesh_builder
	{
		static_vector<glm::vec3, MAX_CAPACITY> vertices;
		static_vector<glm::vec3, MAX_CAPACITY> colors;
		static_vector<glm::vec2, MAX_CAPACITY> uvs;
	public:
		void add_vertex(glm::vec3 vertex, glm::vec2 uv, glm::vec3 color = { 1.f, 1.f, 1.f }) override {
			vertices.push_back(vertex);
			uvs.push_back(uv);
			colors.push_back(color);
		}
		size_t count() const override
		{
			return vertices.size();
		}

		const glm::vec3* get_vertices() const override
		{
			return &vertices[0];
		}
		const glm::vec3* get_colors() const override
		{
			return &colors[0];
		}
		const glm::vec2* get_uvs() const override
		{
			return &uvs[0];
		}
	};

	class dynamic_mesh_builder : public mesh_builder
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> colors;
		std::vector<glm::vec2> uvs;
	public:
		explicit dynamic_mesh_builder(std::size_t t);
		void add_vertex(glm::vec3 vertex, glm::vec2 uv, glm::vec3 color = { 1.f, 1.f, 1.f }) override {
			vertices.push_back(vertex);
			uvs.push_back(uv);
			colors.push_back(color);
		}
		size_t count() const override
		{
			return vertices.size();
		}
		const glm::vec3* get_vertices() const override
		{
			return &vertices[0];
		}
		const glm::vec3* get_colors() const override
		{
			return &colors[0];
		}
		const glm::vec2* get_uvs() const override
		{
			return &uvs[0];
		}
	};

	class mesh {
		friend mesh_builder;
		gl::buffer vertices{};
		gl::buffer uvs{};
		gl::buffer colors{};
		std::size_t count{};

		void resize(std::size_t size) noexcept;

	public:
		mesh() = default;
		mesh(std::size_t size);
		mesh(gl::buffer&& vertices, gl::buffer&& uvs, gl::buffer&& colors, std::size_t count) noexcept;

		void update(const glm::vec3* vertices, const glm::vec3* colors, const glm::vec2* uvs, std::size_t size, std::size_t offset = 0);
		void render() const noexcept;
	};

	void make_cube(mesh_builder &builder, float size, glm::vec3 color, glm::vec3 position = {});

	mesh make_cube(float size, glm::vec3 color, glm::vec3 position = {});

	void make_circle(mesh_builder &builder, glm::vec3 color, float resolution, const bounding_box<float>& texture_area);

	mesh make_circle(glm::vec3 color, const bounding_box<float>& texture_area);

	void make_square(mesh_builder &builder, glm::vec3 color, const bounding_box<float>& texture_area);

	mesh make_square(glm::vec3 color, const bounding_box<float>& texture_area);
}
#endif
