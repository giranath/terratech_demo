#include "game.hpp"
#include "sdl/sdl.hpp"

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

    // Initializes gl3w with context created in window
    if(gl3wInit() != 0) {
        std::cerr << "cannot initialize gl3w" << std::endl;
        return 1;
    }

    const auto TARGET_FRAME_DURATION = std::chrono::milliseconds(17);

    game::frame_duration last_frame_duration = TARGET_FRAME_DURATION;

    game game_state;

    setup_opengl();

    // Game loop
    while(!game_state.wants_to_die()) {
        const auto start_of_frame = game::clock::now();

        //std::cout << "FPS: " << game_state.fps() << std::endl;

        // Render last frame on screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        game_state.render();
        window.gl_swap();

        // Handle events from user here
        for(auto event : sdl.poll_events()) {
            if(event.type == SDL_QUIT) {
                game_state.kill();
            }
            else {
                game_state.handle_event(event);
            }
        }
        
        // Update state here
        game_state.update(last_frame_duration);

        const auto end_of_frame = game::clock::now();
        last_frame_duration = end_of_frame - start_of_frame;
    }

    return 0;
}