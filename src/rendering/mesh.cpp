#include "mesh.hpp"

void mesh_builder::add_vertex(glm::vec3 vertex, glm::vec3 color) {
    vertices.push_back(std::move(vertex));
    colors.push_back(std::move(color));
}

mesh mesh_builder::build() const noexcept {
    gl::buffer vertices_buffer = gl::buffer::make();
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(vertices_buffer));
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

    gl::buffer colors_buffer = gl::buffer::make();
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(colors_buffer));
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colors.size(), &colors.front(), GL_STATIC_DRAW);

    return mesh{std::move(vertices_buffer), std::move(colors_buffer), vertices.size()};
}

mesh::mesh(gl::buffer&& vertices, gl::buffer&& colors, std::size_t count) noexcept
: vertices{std::move(vertices)}
, colors{std::move(colors)}
, count{count} {

}

void mesh::render() const noexcept {
    glEnableVertexAttribArray(0);
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(vertices));
    glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            nullptr            // array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(colors));
    glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            nullptr                          // array buffer offset
    );

    // Draw floor
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(count));

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

mesh make_cube(float size, glm::vec3 color) {
    mesh_builder cube_builder;

    // Front face
    cube_builder.add_vertex({0.f,  0.f,  0.f},  color);
    cube_builder.add_vertex({size, 0.f,  0.f},  color);
    cube_builder.add_vertex({size, size, 0.f},  color);
    cube_builder.add_vertex({0.f,  0.f,  0.f},  color);
    cube_builder.add_vertex({size, size, 0.f},  color);
    cube_builder.add_vertex({0.f,  size, 0.f},  color);

    // back face
    cube_builder.add_vertex({0.f,  0.f,  size}, color);
    cube_builder.add_vertex({size, 0.f,  size}, color);
    cube_builder.add_vertex({size, size, size}, color);
    cube_builder.add_vertex({0.f,  0.f,  size}, color);
    cube_builder.add_vertex({size, size, size}, color);
    cube_builder.add_vertex({0.f,  size, size}, color);

    // left face
    cube_builder.add_vertex({0.f,  0.f,  0.f},  color);
    cube_builder.add_vertex({0.f,  0.f,  size}, color);
    cube_builder.add_vertex({0.f,  size, size}, color);
    cube_builder.add_vertex({0.f,  0.f,  0.f},  color);
    cube_builder.add_vertex({0.f,  size, size}, color);
    cube_builder.add_vertex({0.f,  size, 0.f},  color);

    // right face
    cube_builder.add_vertex({size, 0.f,  0.f},  color);
    cube_builder.add_vertex({size, 0.f,  size}, color);
    cube_builder.add_vertex({size, size, size}, color);
    cube_builder.add_vertex({size, 0.f,  0.f},  color);
    cube_builder.add_vertex({size, size, size}, color);
    cube_builder.add_vertex({size, size, 0.f},  color);

    // bottom face
    cube_builder.add_vertex({0.f,  0.f, 0.f},   color);
    cube_builder.add_vertex({size, 0.f, 0.f},   color);
    cube_builder.add_vertex({size, 0.f, size},  color);
    cube_builder.add_vertex({0.f,  0.f, 0.f},   color);
    cube_builder.add_vertex({size, 0.f, size},  color);
    cube_builder.add_vertex({0.f,  0.f, size},  color);

    // top face
    cube_builder.add_vertex({0.f,  size, 0.f},  color);
    cube_builder.add_vertex({size, size, 0.f},  color);
    cube_builder.add_vertex({size, size, size}, color);
    cube_builder.add_vertex({0.f,  size, 0.f},  color);
    cube_builder.add_vertex({size, size, size}, color);
    cube_builder.add_vertex({0.f,  size, size}, color);

    return cube_builder.build();
}