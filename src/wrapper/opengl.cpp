#include "opengl.hpp"

namespace gl {

//======================================================================================================================
// VERTEX ARRAY
//======================================================================================================================
vertex_array::vertex_array(GLuint vao) noexcept
: vao{vao} {

}

vertex_array::~vertex_array() noexcept {
    glDeleteVertexArrays(1, &vao);
}

bool vertex_array::good() const noexcept {
    return vao != 0;
}

vertex_array::operator GLuint() const noexcept {
    return vao;
}

void vertex_array::bind() const noexcept {
    glBindVertexArray(vao);
}

vertex_array vertex_array::make() noexcept {
    GLuint vao;
    glGenVertexArrays(1, &vao);

    // TODO: Check generation

    return vertex_array(vao);
}

//======================================================================================================================
// BUFFER
//======================================================================================================================
buffer::buffer(GLuint vbo) noexcept
: vbo{vbo} {

}

buffer::~buffer() noexcept {
    glDeleteBuffers(1, &vbo);
}

bool buffer::good() const noexcept {
    return vbo != 0;
}

buffer::operator GLuint() const noexcept {
    return vbo;
}

void buffer::bind(GLenum target) const noexcept {
    glBindBuffer(target, vbo);
}

buffer buffer::make() noexcept {
    GLuint vbo;
    glGenBuffers(1, &vbo);

    return buffer(vbo);
}
}