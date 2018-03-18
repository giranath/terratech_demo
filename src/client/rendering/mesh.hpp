#ifndef MMAP_DEMO_MESH_HPP
#define MMAP_DEMO_MESH_HPP

#include "../opengl/opengl.hpp"
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

    void add_vertex(glm::vec3 vertex, glm::vec2 uv, glm::vec3 color = {1.f, 1.f, 1.f});

    mesh build() const noexcept;
};

class mesh {
    friend mesh_builder;
    gl::buffer vertices{};
    gl::buffer uvs{};
    gl::buffer colors{};
    std::size_t count{};

    mesh(gl::buffer &&vertices, gl::buffer &&uvs, gl::buffer &&colors, std::size_t count) noexcept;

public:
    mesh() = default;

    void render() const noexcept;
};

void make_cube(mesh_builder &builder, float size, glm::vec3 color, glm::vec3 position = {});

mesh make_cube(float size, glm::vec3 color, glm::vec3 position = {});
}

#endif
