#include "game.hpp"
#include "control/all_commands.hpp"

#include <algorithm>
#include <iterator>
#include <numeric>

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
, world_rendering(game_world)
, game_camera(-400.f, 400.f, -300.f, 300.f, -1000.f, 1000.f)
, is_scrolling(false)
, is_running(true)
, last_fps_duration_index(0)
, frame_count(0)
, last_fps_timepoint(clock::now()) {
    std::fill(std::begin(last_fps_durations), std::end(last_fps_durations), 0);

    // Setup controls
    key_inputs.register_state(SDLK_LEFT, std::make_unique<look_left_command>(&game_camera));
    key_inputs.register_state(SDLK_RIGHT, std::make_unique<look_right_command>(&game_camera));
    key_inputs.register_state(SDLK_UP, std::make_unique<look_up_command>(&game_camera));
    key_inputs.register_state(SDLK_DOWN, std::make_unique<look_down_command>(&game_camera));
    key_inputs.register_state(SDLK_a, std::make_unique<look_left_command>(&game_camera));
    key_inputs.register_state(SDLK_d, std::make_unique<look_right_command>(&game_camera));
    key_inputs.register_state(SDLK_w, std::make_unique<look_up_command>(&game_camera));
    key_inputs.register_state(SDLK_s, std::make_unique<look_down_command>(&game_camera));
    key_inputs.register_action(SDLK_m, KMOD_CTRL, std::make_unique<wireframe_command>());

    // Setup world rendering
    for(int x = 0; x < 20; ++x) {
        for(int z = 0; z < 20; ++z) {
            world_rendering.show(x, z);
        }
    }

    // Setup camera
    game_camera.reset({-100.f, 10.f, -200.f});

    // Setup mesh rendering
    mesh_rendering.set_camera(&game_camera);
    mesh_rendering.set_program(0, load_program("standard"));
    mesh_rendering.set_texture(0, gl::texture::load_from_path("asset/texture/terrain.png"));
    mesh_rendering.set_texture(1, gl::texture{});
}

void game::update(frame_duration last_frame_duration) {
    key_inputs.dispatch();
}

void game::render() {
    world_rendering.render(mesh_rendering);
    mesh_rendering.render();

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
    else if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        key_inputs.handle(event);
    }
}

void game::resize(int new_width, int new_height) {
    glViewport(0, 0, new_width, new_height);

    const float aspect = static_cast<float>(new_width) / new_height;
    if(aspect >= 1.0f) {
        game_camera.adjust(-400.f * aspect, 400.f * aspect, -300.f, 300.f, -1000.f, 1000.f);
    }
    else {
        game_camera.adjust(-400.f, 400.f, -300.f / aspect, 300.f / aspect, -1000.f, 1000.f);
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