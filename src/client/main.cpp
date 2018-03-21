#include "game.hpp"
#include "sdl/sdl.hpp"
#include "debug/profiler.hpp"
#include "../common/time/clock.hpp"

#ifdef WIN32
#include <SDL_net.h>
#else
#include <SDL2/SDL_net.h>
#endif

#include <iostream>
#include <iterator>
#include <chrono>

void setup_opengl() {
    std::cout << "available extensions: " << std::endl;
    gl::get_extensions(std::ostream_iterator<const char*>(std::cout, "\n"));

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    gl::enable_debug_messages();

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
}

void set_opengl_version(int major, int minor) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
}

int main(int argc, char* argv[]) {
    sdl::context<>& sdl = sdl::context<>::instance();
    if(!sdl.good()) {
        std::cerr << "cannot initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Setup OpenGL attributes
    set_opengl_version(3, 3);

    // Setup the window
    sdl::window window("RTS v." GAME_VERSION " (terratech v." TERRA_VERSION_STR ")", 800, 600);
    if(!window.good()) {
        std::cerr << "cannot create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_SetWindowMinimumSize(window, 800, 600);

    // Initializes gl3w with context created in window
    if(gl3wInit() != 0) {
        std::cerr << "cannot initialize gl3w" << std::endl;
        return 1;
    }

    const auto TARGET_FRAME_DURATION = std::chrono::milliseconds(17);

    game::frame_duration last_frame_duration = TARGET_FRAME_DURATION;

    setup_opengl();

    game game_state;
    game_state.resize(800, 600);

    // TODO: Init on another thread
    game_state.init();

    // Game loop
    game_time::highres_clock frame_time;
    while(game_state.is_running()) {
        frame_time.restart();

        if(game_state.fps() > 0) {
            std::cout << game_state.fps() << " : " << game_state.average_fps() << std::endl;
        }
        // Render last frame on screen
        {
            profiler_us p("rendering");
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            game_state.render();
            window.gl_swap();
        }

        // Handle events from user here
        {
            profiler_us p("events");
            for (auto event : sdl.poll_events()) {
                if (event.type == SDL_QUIT) {
                    game_state.stop();
                } else if (event.type == SDL_WINDOWEVENT) {
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        const int new_width = event.window.data1;
                        const int new_height = event.window.data2;

                        game_state.resize(new_width, new_height);
                    }
                } else {
                    game_state.handle_event(event);
                }
            }
        }
        
        // Update state here
        {
            profiler_us p("update");
            game_state.update(last_frame_duration);
        }

        last_frame_duration = frame_time.elapsed_time<game::frame_duration>();
    }

    game_state.release();

    return 0;
}