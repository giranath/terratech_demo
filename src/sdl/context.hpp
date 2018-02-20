#ifndef MMAP_DEMO_CONTEXT_HPP
#define MMAP_DEMO_CONTEXT_HPP

#include <SDL.h>

#include "poll_event_iterator.hpp"

namespace sdl {

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

}

#endif
