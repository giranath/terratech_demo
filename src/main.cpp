#include "opengl/opengl.hpp"
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

template<typename shader_type>
std::unique_ptr<gl::shader> load_shader(std::istream& stream) {
    std::unique_ptr<gl::shader> shader_ptr = std::make_unique<shader_type>();
    shader_ptr->compile(stream);

    return shader_ptr;
}

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

    std::vector<std::unique_ptr<gl::shader>> shaders;
    gl::fragment_shader fragment_shader;

    std::ifstream shader_stream{"src/shader/standard.vert"};
    shaders.push_back(load_shader<gl::vertex_shader>(shader_stream));

    shader_stream = std::ifstream("src/shader/standard.frag");
    shaders.push_back(load_shader<gl::fragment_shader>(shader_stream));

    gl::program shader_prog;
    //std::vector<const gl::shader&> shader_ptrs;
    //std::transform(shaders.begin(), shaders.end(), std::back_inserter(shader_ptrs), [](const auto& shader) {
    //   return *shader.get();
    //});

    //shader_prog.attach(std::begin(shader_ptrs), std::end(shader_ptrs));

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