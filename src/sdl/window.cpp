#include "window.hpp"
#include <algorithm>

namespace sdl {

window::window(const char* title, int width, int height) noexcept
: sdl_window(SDL_CreateWindow(title,
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE))
, gl_ctx{SDL_GL_CreateContext(sdl_window)} {

}

window::window(window&& other) noexcept
        : sdl_window(other.sdl_window) {
    other.sdl_window = nullptr;
}

window& window::operator=(window&& other) noexcept {
    std::swap(sdl_window, other.sdl_window);

    return *this;
}

window::~window() noexcept {
    SDL_GL_DeleteContext(gl_ctx);
    SDL_DestroyWindow(sdl_window);
}

void window::gl_swap() const noexcept {
    SDL_GL_SwapWindow(sdl_window);
}

bool window::good() const noexcept {
    return sdl_window != nullptr && gl_ctx != 0;
}

bool window::gl_good() const noexcept {
    return gl_ctx != 0;
}

window::operator SDL_Window*() {
    return sdl_window;
}

window::operator const SDL_Window*() const {
    return sdl_window;
}

}