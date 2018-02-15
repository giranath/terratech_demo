#include <SDL.h>
#include <mapgen/mapgen.h>
#include <iostream>
#include <iterator>

namespace sdl {

class poll_event_iterator {
    SDL_Event event;
    bool is_last;
public:
    using difference_type = void;
    using value_type = SDL_Event;
    using pointer = const value_type*;
    using reference = const value_type&;
    using iterator_category = std::input_iterator_tag;

    poll_event_iterator()
    : event()
    , is_last(true) {
    }

    explicit poll_event_iterator(SDL_Event event)
    : event(event)
    , is_last(false) {

    }

    poll_event_iterator operator++(int) noexcept {
        auto temp = *this;

        (*this)++;

        return temp;
    }

    poll_event_iterator& operator++() noexcept {
        is_last = SDL_PollEvent(&event) == 0;

        return *this;
    }

    bool operator==(const poll_event_iterator& other) const noexcept {
        return is_last == other.is_last;
    }

    bool operator!=(const poll_event_iterator& other) const noexcept {
        return is_last != other.is_last;
    }

    reference operator*() const noexcept {
        return event;
    }

    pointer operator->() const noexcept {
        return &event;
    }
};

template<uint32_t FLAGS = SDL_INIT_EVERYTHING>
class sdl_context {
    int init_code;
    sdl_context() noexcept
    : init_code(SDL_Init(FLAGS)){
    }

public:
    sdl_context(const sdl_context&) = delete;
    sdl_context& operator=(const sdl_context&) = delete;

    sdl_context(sdl_context&&) = delete;
    sdl_context& operator=(sdl_context&&) = delete;

    ~sdl_context() noexcept {
        SDL_Quit();
    }

    bool good() const noexcept {
        return init_code == 0;
    }

    poll_event_iterator poll_events() const noexcept {
        SDL_Event event;
        if(SDL_PollEvent(&event)) {
            return poll_event_iterator{event};
        }

        return poll_event_iterator{};
    }

    static sdl_context& instance() noexcept {
        static sdl_context ctx;

        return ctx;
    }
};

class window {
    SDL_Window* sdl_window = nullptr;
    SDL_GLContext gl_ctx {};
public:
    window() = default;

    window(const char* title, int width, int height) noexcept
    : sdl_window(SDL_CreateWindow(title,
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height,
                                  SDL_WINDOW_OPENGL))
    , gl_ctx{SDL_GL_CreateContext(sdl_window)} {

    }

    explicit window(window&& other) noexcept
    : sdl_window(other.sdl_window) {
        other.sdl_window = nullptr;
    }

    window& operator=(window&& other) noexcept {
        std::swap(sdl_window, other.sdl_window);

        return *this;
    }

    window(const window&) = delete;
    window& operator=(const window&) = delete;

    ~window() noexcept {
        SDL_GL_DeleteContext(gl_ctx);
        SDL_DestroyWindow(sdl_window);
    }

    void gl_swap() const noexcept {
        SDL_GL_SwapWindow(sdl_window);
    }

    bool good() const noexcept {
        return sdl_window != nullptr && gl_ctx != 0;
    }

    bool gl_good() const noexcept {
        return gl_ctx != 0;
    }

    operator SDL_Window*() {
        return sdl_window;
    }

    operator const SDL_Window*() const {
        return sdl_window;
    }
};
}

int main() {
    sdl::sdl_context<>& sdl = sdl::sdl_context<>::instance();

    if(!sdl.good()) {
        std::cerr << "cannot initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    sdl::window window("RTS v." GAME_VERSION " (mapgen v." MAPGEN_VERSION_STR ")", 800, 600);
    if(!window.good()) {
        std::cerr << "cannot create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    bool is_running = true;
    while(is_running) {
        // Render current state here
        window.gl_swap();

        // Handle events from user here
        std::for_each(sdl.poll_events(), sdl::poll_event_iterator{}, [&is_running](const SDL_Event& event) {
            if(event.type == SDL_QUIT) {
                is_running = false;
            }
        });

        // Update state here
    }

    return 0;
}