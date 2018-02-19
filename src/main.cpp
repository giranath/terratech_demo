#include "wrapper/sdl.hpp"
#include "game.hpp"

#include <mapgen/mapgen.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

int main() {
    sdl::context<>& sdl = sdl::context<>::instance();

    if(!sdl.good()) {
        std::cerr << "cannot initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Setup OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Setup the window
    sdl::window window("RTS v." GAME_VERSION " (mapgen v." MAPGEN_VERSION_STR ")", 800, 600);
    if(!window.good()) {
        std::cerr << "cannot create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    const auto TARGET_FRAME_DURATION = std::chrono::milliseconds(17);

    game::frame_duration last_frame_duration = TARGET_FRAME_DURATION;

    game game_state;

    // Game loop
    bool is_running = true;
    while(is_running) {
        const auto start_of_frame = game::clock::now();
        game_state.render();
        window.gl_swap();

        // Handle events from user here
        for(auto event : sdl.poll_events()) {
            if(event.type == SDL_QUIT) {
                is_running = false;
            }

            // TODO: Dispatch the game events to the game
        }

        // Update state here
        game_state.update(last_frame_duration);

        const auto end_of_frame = game::clock::now();
        last_frame_duration = end_of_frame - start_of_frame;
    }

    return 0;
}