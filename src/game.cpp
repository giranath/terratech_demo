#include "game.hpp"

#include <algorithm>
#include <iterator>
#include <numeric>
#include <memory>

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

game::game()
: tasks(std::thread::hardware_concurrency() - 1)
, game_world(static_cast<uint32_t>(std::time(nullptr)))
, vao(gl::vertex_array::make())
, default_program(load_program("standard"))
, terrain_texture(gl::texture::load_from_path("asset/texture/terrain.png"))
, world_rendering{game_world, terrain_texture}
, game_camera(-400.f, 400.f, -300.f, 300.f, -1000.f, 1000.f)
, is_scrolling(false)
, is_running(true)
, last_fps_duration_index(0)
, frame_count(0)
, last_fps_timepoint(clock::now()) {
    std::fill(std::begin(last_fps_durations), std::end(last_fps_durations), 0);

    for(int x = 0; x < 20; ++x) {
        for(int z = 0; z < 20; ++z) {
            world_rendering.show(x, z);
        }
    }
}

void game::update(frame_duration last_frame_duration) {
}

void game::render() {
    gl::bind(vao);
    gl::bind(default_program);

    auto model_matrix_uniform = default_program.find_uniform<glm::mat4>("model_matrix");
    auto view_matrix_uniform = default_program.find_uniform<glm::mat4>("view_matrix");
    auto projection_matrix_uniform = default_program.find_uniform<glm::mat4>("projection_matrix");
    auto is_textured_uniform = default_program.find_uniform<int>("is_textured");

    glm::mat4 model_matrix{1.f};
    is_textured_uniform.set(0);
    model_matrix_uniform.set(model_matrix);
    view_matrix_uniform.set(game_camera.view());
    projection_matrix_uniform.set(game_camera.projection());

    world_rendering.render(default_program, model_matrix);

    // Calculates FPS
    ++frame_count;
    const clock::time_point now = clock::now();
    if(std::chrono::duration_cast<std::chrono::seconds>(now - last_fps_timepoint).count() >= 1) {
        last_fps_timepoint = now;

        last_fps_durations[last_fps_duration_index++ % last_fps_durations.size()] = frame_count;

        frame_count = 0;
    }
}

void game::handle_event(SDL_Event event) {
    if(event.type == SDL_MOUSEBUTTONDOWN) {
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
            const glm::vec3 right_translation = game_camera.right() * static_cast<float>(event.motion.xrel) * -1.f;
            const glm::vec3 forward_translation = game_camera.forward() * static_cast<float>(event.motion.yrel);
            const glm::vec3 cam_translation = right_translation + forward_translation;

            game_camera.translate(cam_translation);
        }
    }
    else if(event.type == SDL_WINDOWEVENT) {
        if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
            const int new_width = event.window.data1;
            const int new_height = event.window.data2;

            glViewport(0, 0, new_width, new_height);
        }
    }
}

bool game::wants_to_die() const noexcept {
    return !is_running;
}

void game::kill() noexcept {
    is_running = false;
}

int game::fps() const noexcept {
    int sum = std::accumulate(std::begin(last_fps_durations), std::end(last_fps_durations), 0);

    return static_cast<int>(sum / last_fps_durations.size());
}