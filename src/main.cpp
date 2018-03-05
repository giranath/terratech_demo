#include "opengl/opengl.hpp"
#include "sdl/sdl.hpp"
#include "game.hpp"
#include "camera.hpp"
#include "world/world.hpp"
#include "world/world_generator.hpp"
#include "chunk_renderer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <terratech/terratech.h>
#include <fstream>

template<typename Shader>
Shader load_shader(const std::string& name) {
    std::ifstream file("asset/shader/" + name);
    Shader shader;

    auto res = shader.compile(file);
    if(!res.good()) {
        std::cerr << res.message() << std::endl;
        shader = Shader{};
    }
    else if(!res.message().empty()) {
        std::cout << res.message() << std::endl;
    }

    return shader;
}

gl::program load_program(const std::string& name) {
    auto ver = load_shader<gl::vertex_shader>(name + ".vert");
    auto frag = load_shader<gl::fragment_shader>(name + ".frag");

    gl::program prog{};
    prog.attach(ver);
    prog.attach(frag);

    auto res = prog.link();
    if(!res.good()) {
        prog = gl::program{};
        std::cerr << res.message() << std::endl;
    }

    return prog;
}

void setup_opengl() {
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

int main() {
    world game_world(static_cast<uint32_t>(std::time(nullptr)));
    auto chunk = game_world.generate_at(0, 0);

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

    gl::vertex_array vao = gl::vertex_array::make();
    gl::bind(vao);

    gl::program prog = load_program("standard");

    auto model_matrix_uniform = prog.find_uniform<glm::mat4>("model_matrix");
    auto camera_matrix_uniform = prog.find_uniform<glm::mat4>("camera_matrix");

    chunk_renderer chunk_ren{chunk};

    glm::mat4 model_matrix{1.f};

    camera god_cam(-400.f, 400.f, -300.f, 300.f, -1000.f, 1000.f);
    god_cam.reset({200.f, 200.f, 200.f});

    bool show_wireframe = false;
    // Game loop
    bool is_running = true;
    while(is_running) {
        const float LAST_FRAME_DURATION = std::chrono::duration_cast<std::chrono::milliseconds>(last_frame_duration).count() / 1000.f;

        const auto start_of_frame = game::clock::now();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gl::bind(prog);

        model_matrix_uniform.set(model_matrix);
        camera_matrix_uniform.set(god_cam.matrix());

        game_state.render();

        chunk_ren.render();

        window.gl_swap();

        constexpr float CAM_SPEED = 10.f;

        // Handle events from user here
        for(auto event : sdl.poll_events()) {
            if(event.type == SDL_QUIT) {
                is_running = false;
            }
            else if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT:
                        god_cam.translate({-CAM_SPEED, 0.f, 0.f});
                        break;
                    case SDLK_RIGHT:
                        god_cam.translate({CAM_SPEED, 0.f, 0.f});
                        break;
                    case SDLK_UP:
                        god_cam.translate({0.f, CAM_SPEED, 0.f});
                        break;
                    case SDLK_DOWN:
                        god_cam.translate({0.f, -CAM_SPEED, 0.f});
                        break;
                    case SDLK_m:
                        show_wireframe = !show_wireframe;

                        glPolygonMode( GL_FRONT_AND_BACK, show_wireframe ? GL_LINE : GL_FILL);
                        break;
                }
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