#ifndef MMAP_DEMO_OPENGL_HPP
#define MMAP_DEMO_OPENGL_HPP

#include <iostream>
#include <string>
#include <GL/gl3w.h>
#include <SDL_opengl.h>

namespace gl {

// Traits to verify that a resource is bindable
template<typename T>
struct is_bindable {
    static const bool value = false;
};

struct vertex_shader_type {
    static const GLenum value = GL_VERTEX_SHADER;
};
struct fragment_shader_type {
    static const GLenum value = GL_FRAGMENT_SHADER;
};

template<typename T>
struct is_shader_type {
    static const bool value = false;
};

template<>
struct is_shader_type<vertex_shader_type> {
    static const bool value = true;
};

template<>
struct is_shader_type<fragment_shader_type> {
    static const bool value = true;
};

class shader {
    GLuint raw;
protected:
    shader(GLenum type) noexcept;
public:
    class compilation_status {
        std::string msg;
        bool has_failed;
    public:
        compilation_status(bool has_failed, const std::string& msg)
        : msg{msg}
        , has_failed{has_failed} {

        }

        bool good() const noexcept {
            return !has_failed;
        }

        operator bool() const noexcept {
            return good();
        }

        const std::string& message() const noexcept {
            return msg;
        }
    };

    // Copy is disabled
    shader(const shader&) = delete;
    shader& operator=(const shader&) = delete;

    // Move is enabled
    shader(shader&&) noexcept;
    shader& operator=(shader&&) noexcept;

    virtual ~shader() noexcept;

    compilation_status compile(const char* source) const noexcept;
    compilation_status compile(const std::string& source) const noexcept;
    compilation_status compile(std::istream& source_stream) const noexcept;

    operator GLuint() const noexcept;
};

template<typename TYPE>
class typed_shader : public shader {
    static_assert(is_shader_type<TYPE>::value, "you must specify a valid shader type");
public:
    using type = TYPE;

    typed_shader() noexcept
    : shader(TYPE::value) {

    }
};

using vertex_shader = typed_shader<vertex_shader_type>;
using fragment_shader = typed_shader<fragment_shader_type>;

template<typename T>
struct is_shader {
    static const bool value = false;
};

template<>
struct is_shader<shader> {
    static const bool value = true;
};

template<typename T>
struct is_shader<typed_shader<T>> {
    static const bool value = true;
};

class program {
    GLuint prog;
public:
    class link_status {
        bool is_linked;
        std::string msg;
    public:
        link_status(bool is_linked, const std::string& msg);

        bool good() const noexcept;
        operator bool() const noexcept;

        const std::string& message() const noexcept;
    };

    program() noexcept;

    program(const program&) = delete;
    program& operator=(const program&) = delete;

    program(program&&) noexcept;
    program& operator=(program&&) noexcept;

    ~program() noexcept;

    template<typename SHADER>
    void attach(const SHADER& shader) const noexcept {
        static_assert(is_shader<SHADER>::value, "you must specify a shader type");

        glAttachShader(prog, shader);
    }

    template<typename It>
    void attach(It begin, It end) const noexcept {
        static_assert(is_shader<typename It::value_type>::value, "you must specify an iterator on shader");

        std::for_each(begin, end, [this](const typename It::value_type& shader){
            attach(shader);
        });
    }

    operator GLuint() const noexcept;

    link_status link() const noexcept;

    void bind() const noexcept;
};

template<>
struct is_bindable<program> {
    static const bool value = true;
};

class vertex_array {
    GLuint vao {};

    explicit vertex_array(GLuint vao) noexcept;
public:
    // By default, a vertex_array is invalid
    vertex_array() noexcept = default;

    // A vertex array can be moved
    vertex_array(vertex_array&&) noexcept;
    vertex_array& operator=(vertex_array&&) noexcept;

    // A vertex array cannot be copied
    vertex_array(const vertex_array&) = delete;
    vertex_array& operator=(const vertex_array&) = delete;

    ~vertex_array() noexcept;

    bool good() const noexcept;
    operator GLuint() const noexcept;

    void bind() const noexcept;

    // Factory function to create valid vao
    static vertex_array make() noexcept;
};

template<>
struct is_bindable<vertex_array> {
    static const bool value = true;
};

class buffer {
    GLuint vbo {};

    explicit buffer(GLuint vbo) noexcept;
public:
    buffer() noexcept = default;

    buffer(const buffer&) = delete;
    buffer& operator=(const buffer&) = delete;

    buffer(buffer&&) noexcept;
    buffer& operator=(buffer&&) noexcept;

    ~buffer() noexcept;

    bool good() const noexcept;
    operator GLuint() const noexcept;

    void bind(GLenum target) const noexcept;

    static buffer make() noexcept;
};

template<GLenum TARGET>
class buffer_bind {
    const buffer& buf;
public:
    explicit buffer_bind(const buffer& buf)
    : buf{buf} {

    }

    void bind() const noexcept {
        buf.bind(TARGET);
    }
};

template<GLenum TARGET>
struct is_bindable<buffer_bind<TARGET>> {
    static const bool value = true;
};

// Bind an OpenGL resource
template<typename T>
void bind(const T& t) noexcept {
    static_assert(is_bindable<T>::value, "type must be bindable to call bind");
    t.bind();
}

}

#endif
