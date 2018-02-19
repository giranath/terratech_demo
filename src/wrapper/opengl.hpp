#ifndef MMAP_DEMO_OPENGL_HPP
#define MMAP_DEMO_OPENGL_HPP

#include <GL/gl3w.h>
#include <SDL_opengl.h>

namespace gl {

// Traits to verify that a resource is bindable
template<typename T>
struct is_bindable {
    static const bool value = false;
};

class vertex_array {
    GLuint vao {};

    explicit vertex_array(GLuint vao) noexcept;
public:
    // By default, a vertex_array is invalid
    vertex_array() noexcept = default;

    // A vertex array can be moved
    vertex_array(vertex_array&&) noexcept = default;
    vertex_array& operator=(vertex_array&&) noexcept = default;

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

    buffer(buffer&&) noexcept = default;
    buffer& operator=(buffer&&) noexcept = default;

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
