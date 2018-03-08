#ifndef MMAP_DEMO_TEXTURE_H
#define MMAP_DEMO_TEXTURE_H

#include <GL/gl3w.h>
#include <SDL_opengl.h>

#include "bind.hpp"

namespace gl {

class texture {
    GLuint raw;
    explicit texture(GLuint raw) noexcept;
public:
    texture() noexcept;
    texture(const texture&) = delete;
    texture& operator=(const texture&) = delete;
    texture(texture&& other) noexcept;
    texture& operator=(texture&& other) noexcept;
    ~texture() noexcept;

    operator GLuint() const noexcept;

    static texture make() noexcept;
};

template<GLenum TARGET>
class texture_bind {
    const texture& tex;
public:
    explicit texture_bind(const texture& tex)
    : tex{tex} {

    }

    void bind() const noexcept {
        glBindTexture(TARGET, tex);
    }
};

template<GLenum TARGET>
struct is_bindable<texture_bind<TARGET>> {
    static const bool value = true;
};

}

#endif
