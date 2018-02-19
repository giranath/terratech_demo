#ifndef MMAP_DEMO_SDL_HPP
#define MMAP_DEMO_SDL_HPP

#include <SDL.h>
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

    poll_event_iterator();
    explicit poll_event_iterator(SDL_Event event);

    poll_event_iterator operator++(int) noexcept;
    poll_event_iterator& operator++() noexcept;

    bool operator==(const poll_event_iterator& other) const noexcept;
    bool operator!=(const poll_event_iterator& other) const noexcept;

    reference operator*() const noexcept;
    pointer operator->() const noexcept;
};

template<uint32_t FLAGS = SDL_INIT_EVERYTHING>
class context {
    int init_code;
    context() noexcept
    : init_code(SDL_Init(FLAGS)){
    }

public:
    struct events_poller {
        poll_event_iterator begin() const {
            SDL_Event event;
            if(SDL_PollEvent(&event)) {
                return poll_event_iterator{event};
            }

            return end();
        }

        poll_event_iterator end() const {
            return poll_event_iterator{};
        }
    };

    context(const context&) = delete;
    context& operator=(const context&) = delete;

    context(context&&) = delete;
    context& operator=(context&&) = delete;

    ~context() noexcept {
        SDL_Quit();
    }

    bool good() const noexcept {
        return init_code == 0;
    }

    events_poller poll_events() const noexcept {
        return events_poller{};
    }

    static context& instance() noexcept {
        static context ctx;

        return ctx;
    }
};

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

    operator SDL_Window*();
    operator const SDL_Window*() const;
};
}
#endif //MMAP_DEMO_SDL_HPP
