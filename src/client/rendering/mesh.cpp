#include "mesh.hpp"

namespace rendering {

dynamic_mesh_builder::dynamic_mesh_builder(std::size_t capacity) {
    vertices.reserve(capacity);
    uvs.reserve(capacity);
    colors.reserve(capacity);
}

mesh mesh_builder::build() const noexcept {
    if (count() == 0)
        return mesh{};

    gl::buffer vertices_buffer = gl::buffer::make();
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(vertices_buffer));
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * count(), get_vertices(), GL_STATIC_DRAW);

    gl::buffer colors_buffer = gl::buffer::make();
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(colors_buffer));
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * count(), get_colors(), GL_STATIC_DRAW);

    gl::buffer uvs_buffer = gl::buffer::make();
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(uvs_buffer));
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * count(), get_uvs(), GL_STATIC_DRAW);

    return mesh{std::move(vertices_buffer), std::move(uvs_buffer), std::move(colors_buffer), count()};
}

void mesh_builder::rebuild(mesh& m) const noexcept {
    if(count() > 0) {
        m.update(get_vertices(), get_colors(), get_uvs(), count());
        m.resize(count());
    }
}

mesh::mesh(gl::buffer&& vertices, gl::buffer&& uvs, gl::buffer&& colors, std::size_t count) noexcept
: vertices{std::move(vertices)}
, uvs{std::move(uvs)}
, colors{std::move(colors)}
, count{count} {

}

mesh::mesh(std::size_t size)
: vertices(gl::buffer::make())
, uvs(gl::buffer::make())
, colors(gl::buffer::make())
, count(size) {
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(vertices));
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * count, NULL, GL_STREAM_DRAW);

    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(colors));
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * count, NULL, GL_STREAM_DRAW);

    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(uvs));
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * count, NULL, GL_STREAM_DRAW);
}

void mesh::update(const glm::vec3* vertices_, const glm::vec3* colors_, const glm::vec2* uvs_, std::size_t size, std::size_t offset) {
    assert(offset + size <= count);

    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(vertices));
    glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(glm::vec3), size * sizeof(glm::vec3), vertices_);

    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(colors));
    glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(glm::vec3), size * sizeof(glm::vec3), colors_);

    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(uvs));
    glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(glm::vec2), size * sizeof(glm::vec2), uvs_);
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

    glEnableVertexAttribArray(2);
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(uvs));
    glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            0,
            nullptr
    );

    // Draw floor
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(count));

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

void make_circle(mesh_builder &builder, float radius, glm::vec3 color, float resolution, const bounding_box<float>& texture_area) {

	int actual_resolution = resolution;
	float step = 360 / actual_resolution;

	for (int i = 0; i <= actual_resolution; i++) {
		float offset_sin_cur = glm::sin(glm::radians(i * step)) * radius;
		float offset_cos_cur = glm::cos(glm::radians(i * step)) * radius;

		float offset_sin_next = glm::sin(glm::radians((i + 1) * step)) * radius;
		float offset_cos_next = glm::cos(glm::radians((i + 1) * step)) * radius;

		float uv_sin_cur = (offset_sin_cur / radius / 2) + 0.5f;
		float uv_cos_cur = (offset_cos_cur / radius / 2) + 0.5f;

		float uv_sin_next = (offset_sin_next / radius / 2) + 0.5f;
		float uv_cos_next = (offset_cos_next / radius / 2) + 0.5f;

		uv_sin_cur = uv_sin_cur * texture_area.height() + texture_area.bottom();
		uv_cos_cur = uv_cos_cur * texture_area.width() + texture_area.left();

		uv_sin_next = uv_sin_next * texture_area.height() + texture_area.bottom();
		uv_cos_next = uv_cos_next * texture_area.width() + texture_area.left();

		builder.add_vertex({offset_sin_cur, 0, offset_cos_cur}, { uv_cos_cur, uv_sin_cur }, color);
		builder.add_vertex({offset_sin_next, 0, offset_cos_next }, { uv_cos_next, uv_sin_next }, color);
		builder.add_vertex({0, 0, 0}, { texture_area.width() / 2 + texture_area.left(), texture_area.height() / 2 + texture_area.bottom() }, color);
	}
}


mesh make_circle(float radius, glm::vec3 color, float resolution, const bounding_box<float>& texture_area) {
	mesh_builder builder;

	make_circle(builder, radius, color, resolution, texture_area);

	return builder.build();
}
void mesh::resize(std::size_t size) noexcept {
    count = size;
}

void make_cube(mesh_builder &cube_builder, float size, glm::vec3 color, glm::vec3 position) {
    // Front face
    cube_builder.add_vertex({position.x, position.y, position.z}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y, position.z}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y + size, position.z}, {}, color);
    cube_builder.add_vertex({position.x, position.y, position.z}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y + size, position.z}, {}, color);
    cube_builder.add_vertex({position.x, position.y + size, position.z}, {}, color);

    // back face
    cube_builder.add_vertex({position.x, position.y, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y + size, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x, position.y, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y + size, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x, position.y + size, position.z + size}, {}, color);

    // left face
    cube_builder.add_vertex({position.x, position.y, position.z}, {}, color);
    cube_builder.add_vertex({position.x, position.y, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x, position.y + size, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x, position.y, position.z}, {}, color);
    cube_builder.add_vertex({position.x, position.y + size, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x, position.y + size, position.z}, {}, color);

    // right face
    cube_builder.add_vertex({position.x + size, position.y, position.z}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y + size, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y, position.z}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y + size, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y + size, position.z}, {}, color);

    // bottom face
    cube_builder.add_vertex({position.x, position.y, position.z}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y, position.z}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x, position.y, position.z}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x, position.y, position.z + size}, {}, color);

    // top face
    cube_builder.add_vertex({position.x, position.y + size, position.z}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y + size, position.z}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y + size, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x, position.y + size, position.z}, {}, color);
    cube_builder.add_vertex({position.x + size, position.y + size, position.z + size}, {}, color);
    cube_builder.add_vertex({position.x, position.y + size, position.z + size}, {}, color);
}

mesh make_cube(float size, glm::vec3 color, glm::vec3 position) {
    static_mesh_builder<6*6> builder;

    make_cube(builder, size, color, position);

    return builder.build();
}

}