#include "opengl.hpp"

namespace gl {

//======================================================================================================================
// VERTEX ARRAY
//======================================================================================================================
vertex_array::vertex_array(GLuint vao) noexcept
: vao{vao} {

}

vertex_array::vertex_array(vertex_array&& other) noexcept
: vao{other.vao} {

}

vertex_array& vertex_array::operator=(vertex_array&& other) noexcept {
    std::swap(vao, other.vao);

    return *this;
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

buffer::buffer(buffer&& other) noexcept
: vbo{other.vbo} {
    other.vbo = 0;
}

buffer& buffer::operator=(buffer&& other) noexcept {
    std::swap(vbo, other.vbo);
    return *this;
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

//======================================================================================================================
// SHADER
//======================================================================================================================
shader::shader(GLenum type) noexcept
: raw(glCreateShader(type)){

}

shader::shader(shader&& other) noexcept
: raw(other.raw){
    other.raw = 0;
}

shader& shader::operator=(shader&& other) noexcept {
    std::swap(raw, other.raw);

    return *this;
}

shader::~shader() noexcept {
    glDeleteShader(raw);
}

shader::compilation_status shader::compile(std::istream& source_stream) const noexcept {
    std::string source;
    char letter;
    while(source_stream.get(letter)) {
        source.push_back(letter);
    }

    return compile(source);
}

shader::compilation_status shader::compile(const char* source) const noexcept {
    bool compiled = false;

    // Load the sources into the shader
    glShaderSource(raw, 1, &source, nullptr);

    // Compile the shader
    glCompileShader(raw);
    GLint is_compiled;
    glGetShaderiv(raw, GL_COMPILE_STATUS, &is_compiled);

    GLsizei log_len = 0;
    glGetShaderiv(raw, GL_INFO_LOG_LENGTH, &log_len);

    std::string message;
    GLchar* message_buffer = new(std::nothrow) GLchar[log_len];
    if(message_buffer) {
        glGetShaderInfoLog(raw, log_len, nullptr, message_buffer);
        message = std::string{message_buffer};
        delete[] message_buffer;
    }

    return compilation_status{is_compiled == GL_TRUE, message};
}

shader::compilation_status shader::compile(const std::string& source) const noexcept {
    return compile(source.c_str());
}

shader::operator GLuint() const noexcept {
    return raw;
}

//======================================================================================================================
// PROGRAM
//======================================================================================================================
program::link_status::link_status(bool is_linked, const std::string& msg)
: is_linked{is_linked}
, msg{msg} {

}

bool program::link_status::good() const noexcept {
    return is_linked;
}

program::link_status::operator bool() const noexcept {
    return good();
}

const std::string& program::link_status::message() const noexcept {
    return msg;
}

program::program() noexcept
: prog{glCreateProgram()}{

}

program::program(program&& other) noexcept
: prog{other.prog} {
    other.prog = 0;
}

program& program::operator=(program&& other) noexcept {
    std::swap(prog, other.prog);

    return *this;
}

program::~program() noexcept {
    glDeleteProgram(prog);
}

program::operator GLuint() const noexcept {
    return prog;
}

program::link_status program::link() const noexcept {
    glLinkProgram(prog);

    GLint status;
    glGetProgramiv(prog, GL_LINK_STATUS, &status);

    GLsizei log_len = 0;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &log_len);

    std::string message;
    GLchar* message_buffer = new(std::nothrow) GLchar[log_len];
    if(message_buffer) {
        glGetProgramInfoLog(prog, log_len, nullptr, message_buffer);
        message = std::string{message_buffer};
        delete[] message_buffer;
    }

    return link_status{status == GL_TRUE, ""};
}

void program::bind() const noexcept {
    glUseProgram(prog);
}

}