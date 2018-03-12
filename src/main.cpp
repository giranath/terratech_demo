#include "opengl/opengl.hpp"
#include "sdl/sdl.hpp"
#include "game.hpp"
#include "rendering/camera.hpp"
#include "debug/profiler.hpp"
#include "debug/profiler_administrator.hpp"
#include "control/input_handler.hpp"
#include "world/world.hpp"
#include "world/world_generator.hpp"
#include "rendering/chunk_renderer.hpp"
#include "rendering/world_renderer.hpp"
#include "rendering/mesh.hpp"

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
    world game_world(static_cast<uint32_t>(std::time(nullptr)));

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
    gl::program billboard_prog = load_program("billboard");

    gl::texture terrain_texture = gl::texture::load_from_path("asset/texture/terrain.png");
    world_renderer world_render{game_world, terrain_texture};

    for(int x = 0; x < 20; ++x) {
        for(int z = 0; z < 20; ++z) {
            world_render.show(x, z);
        }
    }

    const glm::vec3 cube_color{0.f, 178.f / 255.f, 127.f / 255.f};
    mesh cube_mesh = make_cube(chunk_renderer::SQUARE_SIZE * 0.65f, cube_color);

    const float size = 25.f; glm::vec3 color{1.f, 1.f, 1.f};
    mesh_builder textured_cube_builder;
    textured_cube_builder.add_vertex({0.f,  0.f,  0.f}, {0.f, 0.f}, color);
    textured_cube_builder.add_vertex({size, 0.f,  0.f}, {1.f, 0.f}, color);
    textured_cube_builder.add_vertex({size, size, 0.f}, {1.f, 1.f}, color);
    textured_cube_builder.add_vertex({0.f,  0.f,  0.f}, {0.f, 0.f}, color);
    textured_cube_builder.add_vertex({size, size, 0.f}, {1.f, 1.f}, color);
    textured_cube_builder.add_vertex({0.f,  size, 0.f}, {0.f, 1.f}, color);

    mesh textured_cube = textured_cube_builder.build();

    gl::texture texture = gl::texture::load_from_path("asset/texture/comparator.png");

    const float CAMERA_RATIO = 600.f / 800.f;
    camera god_cam(-400.f, 400.f, -300.f, 300.f, -1000.f, 1000.f);
    god_cam.reset({200.f, 200.f, 200.f});

    const float CAMERA_SPEED = 250.f; // 250 pixels per seconds

    input_handler input(god_cam);
    // Game loop
    bool is_running = true;
    bool is_scrolling = false;
    while(is_running) {
        const float LAST_FRAME_DURATION = std::chrono::duration_cast<std::chrono::milliseconds>(last_frame_duration).count() / 1000.f;

        const auto start_of_frame = game::clock::now();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gl::bind(prog);

        auto model_matrix_uniform = prog.find_uniform<glm::mat4>("model_matrix");
        auto view_matrix_uniform = prog.find_uniform<glm::mat4>("view_matrix");
        auto projection_matrix_uniform = prog.find_uniform<glm::mat4>("projection_matrix");
        auto is_textured_uniform = prog.find_uniform<int>("is_textured");

        glm::mat4 model_matrix{1.f};
        is_textured_uniform.set(0);
        model_matrix_uniform.set(model_matrix);
        view_matrix_uniform.set(god_cam.view());
        projection_matrix_uniform.set(god_cam.projection());

        game_state.render();

        world_render.render(prog, model_matrix);

        gl::bind(billboard_prog);
        model_matrix_uniform = billboard_prog.find_uniform<glm::mat4>("model_matrix");
        view_matrix_uniform = billboard_prog.find_uniform<glm::mat4>("view_matrix");
        projection_matrix_uniform = billboard_prog.find_uniform<glm::mat4>("projection_matrix");
        is_textured_uniform = billboard_prog.find_uniform<int>("is_textured");

        is_textured_uniform.set(1);
        model_matrix_uniform.set(model_matrix);
        view_matrix_uniform.set(god_cam.view());
        projection_matrix_uniform.set(god_cam.projection());

        glActiveTexture(GL_TEXTURE0);
        gl::bind(gl::texture_bind<GL_TEXTURE_2D>(texture));
        model_matrix_uniform.set(glm::mat4{1.f});
        textured_cube.render();

        window.gl_swap();

        constexpr float CAM_SPEED = 10.f;

        // Handle events from user here
        for(auto event : sdl.poll_events()) {
            profiler<std::chrono::nanoseconds> p("test");
            
            if(event.type == SDL_QUIT) {
                is_running = false;
            }
            else if(event.type == SDL_MOUSEBUTTONDOWN) {
                if(event.button.button == SDL_BUTTON_MIDDLE) {
                    is_scrolling = true;
                }
            }
            else if(event.type == SDL_MOUSEBUTTONUP) {
                if(event.button.button == SDL_BUTTON_MIDDLE) {
                    is_scrolling = false;
                }
            }
            else if(event.type == SDL_MOUSEMOTION) {
                if(is_scrolling) {
                    const glm::vec3 right_translation = god_cam.right() * static_cast<float>(event.motion.xrel) * -1.f;
                    const glm::vec3 forward_translation = god_cam.forward() * static_cast<float>(event.motion.yrel);
                    const glm::vec3 cam_translation = right_translation + forward_translation;

                    god_cam.translate(cam_translation);
                }
            }
            else if(event.type == SDL_WINDOWEVENT) {
                if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    const int new_width = event.window.data1;
                    const int new_height = event.window.data2;

                    glViewport(0, 0, new_width, new_height);
                }
            }

            else {
                input.handle_input(event);
            }
        }
        
        // Update state here
        game_state.update(last_frame_duration);

        const auto end_of_frame = game::clock::now();
        last_frame_duration = end_of_frame - start_of_frame;
    }

    return 0;
}