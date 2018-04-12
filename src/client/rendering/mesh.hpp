#ifndef MMAP_DEMO_MESH_HPP
#define MMAP_DEMO_MESH_HPP

#include "../opengl/opengl.hpp"
#include "../../common/bounding_box.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace rendering {

class mesh;

class mesh_builder {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec2> uvs;
public:
    mesh_builder() = default;
    mesh_builder(std::size_t capacity);

    void add_vertex(glm::vec3 vertex, glm::vec2 uv, glm::vec3 color = {1.f, 1.f, 1.f});

    mesh build() const noexcept;
    void rebuild(mesh& m) const noexcept;
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

void make_circle(mesh_builder &builder, float radius, glm::vec3 color, float resolution, const bounding_box<float>& texture_area);

mesh make_circle(float radius, glm::vec3 color, float resolution, const bounding_box<float>& texture_area);
}

#endif
