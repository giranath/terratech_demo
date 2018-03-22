#ifndef MMAP_DEMO_CONTEXT_HPP
#define MMAP_DEMO_CONTEXT_HPP

#include <SDL.h>
#ifdef WIN32
#include <SDL_image.h>
#include <SDL_net.h>
#else
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#endif

#include "poll_event_iterator.hpp"

namespace sdl {

static const int SDL_IMG_INIT_FLAGS = IMG_INIT_PNG;

template<uint32_t FLAGS = SDL_INIT_EVERYTHING>
class context {
    int init_code;
    int img_init_code;
    int net_init_code;
    context() noexcept
    : init_code(SDL_Init(FLAGS))
    , img_init_code(IMG_Init(SDL_IMG_INIT_FLAGS))
    , net_init_code(SDLNet_Init()){
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
        SDLNet_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    bool good() const noexcept {
        return init_code == 0 && img_init_code == SDL_IMG_INIT_FLAGS && net_init_code == 0;
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
