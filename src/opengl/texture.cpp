#include "texture.hpp"
#include "../sdl/istream_rwops.hpp"

#include <algorithm>
#include <SDL2/SDL_image.h>

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

void texture::swap(texture& other) noexcept {
    std::swap(raw, other.raw);
}

texture::operator GLuint() const noexcept {
    return raw;
}

void texture::bind(GLenum target) const noexcept {
    glBindTexture(target, raw);
}

texture texture::make() noexcept {
    GLuint text;
    glGenTextures(1, &text);

    return texture{text};
}

texture texture::load_from_fstream(std::fstream&& stream) noexcept {
    SDL_Surface* surface = IMG_Load_RW(sdl::make_from_fstream(std::move(stream)), 1);
    if(surface) {
        texture tex = make();

        int mode = GL_RGB;
        if(surface->format->BytesPerPixel == 4) {
            mode = GL_RGBA;
        }

        tex.bind(GL_TEXTURE_2D);
        glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return tex;
    }

    return texture{};
}

}