#ifndef MMAP_DEMO_MESH_HPP
#define MMAP_DEMO_MESH_HPP

#include "../opengl/opengl.hpp"
#include <glm/glm.hpp>
#include <vector>

class mesh;

class mesh_builder {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> colors;
public:
    mesh_builder() = default;
    void add_vertex(glm::vec3 vertex, glm::vec3 color);

    mesh build() const noexcept;
};

class mesh {
    friend mesh_builder;
    gl::buffer vertices {};
    gl::buffer colors {};
    std::size_t count {};

    mesh(gl::buffer&& vertices, gl::buffer&& colors, std::size_t count) noexcept;
public:
    mesh() = default;

    void render() const noexcept;
};

mesh make_cube(float size, glm::vec3 color);

#endif
