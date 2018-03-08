#include "texture.hpp"

#include <algorithm>

namespace gl {

texture::texture() noexcept
: raw{0} {

}

texture::texture(GLuint raw) noexcept
: raw{raw} {

}

texture::texture(texture&& other) noexcept
: raw(other.raw) {
    other.raw = 0;
}

texture::~texture() noexcept {
    glDeleteTextures(1, &raw);
}

texture& texture::operator=(texture&& other) noexcept {
    std::swap(raw, other.raw);

    return *this;
}

texture::operator GLuint() const noexcept {
    return raw;
}

texture texture::make() noexcept {
    GLuint text;
    glGenTextures(1, &text);

    return texture{text};
}

}