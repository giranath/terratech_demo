#include "wrapper/opengl.hpp"
#include "wrapper/sdl.hpp"
#include "game.hpp"

#include <mapgen/mapgen.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <fstream>
#include <vector>
#include <memory>

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

    // Initializes gl3w with context created in window
    if(gl3wInit() != 0) {
        std::cerr << "cannot initialize gl3w" << std::endl;
        return 1;
    }

    gl::vertex_array vao = gl::vertex_array::make();

    gl::bind(vao);

    gl::buffer vbo = gl::buffer::make();

    std::vector<gl::shader> shaders;
    gl::vertex_shader vertex_shader;
    gl::fragment_shader fragment_shader;

    std::ifstream shader_stream{"src/shader/standard.vert"};
    auto vert_status = vertex_shader.compile(shader_stream);
    std::cout << "vertex: " << vert_status.message() << std::endl;
    shaders.push_back(std::move(vertex_shader));

    shader_stream = std::ifstream("src/shader/standard.frag");
    auto frag_status = fragment_shader.compile(shader_stream);
    std::cout << "fragment: " << frag_status.message() << std::endl;
    shaders.push_back(std::move(fragment_shader));

    gl::program shader_prog;
    shader_prog.attach(shaders.begin(), shaders.end());

    const auto TARGET_FRAME_DURATION = std::chrono::milliseconds(17);

    game::frame_duration last_frame_duration = TARGET_FRAME_DURATION;

    game game_state;

    // Game loop
    bool is_running = true;
    while(is_running) {
        const auto start_of_frame = game::clock::now();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl::bind(shader_prog);
        gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>{vbo});

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