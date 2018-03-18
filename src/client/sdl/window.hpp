#ifndef MMAP_DEMO_WINDOW_HPP
#define MMAP_DEMO_WINDOW_HPP

#include <SDL.h>

namespace sdl {

class window {
    SDL_Window* sdl_window = nullptr;
    SDL_GLContext gl_ctx {};
public:
    window() = default;
    window(const char* title, int width, int height) noexcept;

    window(window&& other) noexcept;
    window& operator=(window&& other) noexcept;

    window(const window&) = delete;
    window& operator=(const window&) = delete;
    ~window() noexcept;

    void gl_swap() const noexcept;

    bool good() const noexcept;
    bool gl_good() const noexcept;

    void close() noexcept;

    operator SDL_Window*();
    operator const SDL_Window*() const;
};

}

#endif
