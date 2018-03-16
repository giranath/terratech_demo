#include "game.hpp"
#include "control/all_commands.hpp"
#include "debug/profiler.hpp"

#include "actor/unit.hpp"
#include "constant/rendering.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <memory>

// TODO: Include filesystem

int G_TO_REMOVE_SCREEN_WIDTH = 0;
int G_TO_REMOVE_SCREEN_HEIGHT = 0;

template<typename Shader>
Shader load_shader(const std::string& path) {
    std::ifstream file(path);
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

void game::load_flyweight(std::ifstream& stream) {
    using json = nlohmann::json;
    json j = json::parse(stream);

    int id = j["id"];

    unit_flyweights[id] = unit_flyweight(j);
}

void game::load_flyweights() {
    std::ifstream units_list_stream("asset/data/unit.list");

    std::vector<std::string> units_to_load;
    std::copy(std::istream_iterator<std::string>(units_list_stream), std::istream_iterator<std::string>(),
              std::back_inserter(units_to_load));

    std::cout << "will load units" << std::endl;
    std::for_each(std::begin(units_to_load), std::end(units_to_load), [this](const std::string& rel_path) {
        std::string full_path = "asset/data/" + rel_path;

        std::ifstream unit_stream(full_path);
        if(unit_stream.is_open()) {
            std::cout << "loading " << full_path << std::endl;
            load_flyweight(unit_stream);
        }
        else {
            std::cerr << "cannot open " << full_path << std::endl;
        }
    });
}

void game::setup_inputs() {
    // Camera movements
    key_inputs.register_state(SDLK_LEFT, std::make_unique<input::look_left_command>(game_camera, 10.f));
    key_inputs.register_state(SDLK_RIGHT, std::make_unique<input::look_right_command>(game_camera, 10.f));
    key_inputs.register_state(SDLK_UP, std::make_unique<input::look_up_command>(game_camera, 10.f));
    key_inputs.register_state(SDLK_DOWN, std::make_unique<input::look_down_command>(game_camera, 10.f));
    key_inputs.register_state(SDLK_a, std::make_unique<input::look_left_command>(game_camera, 10.f));
    key_inputs.register_state(SDLK_d, std::make_unique<input::look_right_command>(game_camera, 10.f));
    key_inputs.register_state(SDLK_w, std::make_unique<input::look_up_command>(game_camera, 10.f));
    key_inputs.register_state(SDLK_s, std::make_unique<input::look_down_command>(game_camera, 10.f));
    // Wireframe
    key_inputs.register_action(SDLK_m, KMOD_CTRL, std::make_unique<input::wireframe_command>());
}

struct shader_list_record {
    int id;
    std::string vertex_path;
    std::string fragment_path;
};

std::istream& operator>>(std::istream& stream, shader_list_record& record) {
    return stream >> record.id >> record.vertex_path >> record.fragment_path;
}

struct texture_list_record {
    int id;
    std::string path;
};

std::istream& operator>>(std::istream& stream, texture_list_record& record) {
    return stream >> record.id >> record.path;
}

void game::setup_renderer() {
    mesh_rendering.set_camera(&game_camera);

    load_shaders();
    load_textures();
}

void game::load_textures() {
    std::ifstream texture_list_stream("asset/data/texture.list");
    std::vector<texture_list_record> texture_records;
    std::copy(std::istream_iterator<texture_list_record>(texture_list_stream),
              std::istream_iterator<texture_list_record>(),
              std::back_inserter(texture_records));

    std::for_each(std::begin(texture_records), std::end(texture_records), [this](const texture_list_record& record) {
        if(record.path != "NONE") {
            std::string fullpath = "asset/texture/" + record.path;
            gl::texture texture = gl::texture::load_from_path(fullpath.c_str());

            if (texture.good()) {
                mesh_rendering.set_texture(record.id, std::move(texture));
            }
            else {
                std::cerr << "cannot load texture " << record.path << std::endl;
            }
        }
        else {
            mesh_rendering.set_texture(record.id, gl::texture{});
        }
    });
}

void game::load_shaders() {
    std::ifstream program_list_stream("asset/data/shader.list");
    std::vector<shader_list_record> shader_records;
    std::copy(std::istream_iterator<shader_list_record>(program_list_stream),
              std::istream_iterator<shader_list_record>(),
              std::back_inserter(shader_records));

    std::for_each(std::begin(shader_records), std::end(shader_records), [this](const shader_list_record& record) {
        gl::vertex_shader vertex_shader = load_shader<gl::vertex_shader>("asset/shader/" + record.vertex_path);
        gl::fragment_shader fragment_shader = load_shader<gl::fragment_shader>("asset/shader/" + record.fragment_path);

        if(vertex_shader.good() && fragment_shader.good()) {
            gl::program program;
            program.attach(vertex_shader);
            program.attach(fragment_shader);

            auto res = program.link();
            if(res.good()) {
                mesh_rendering.set_program(record.id, std::move(program));
            }
            else {
                std::cerr << "cannot link shader #" << record.id << std::endl;
                std::cerr << res.message() << std::endl;
            }
        }
    });
}

// TODO: REMOVE THIS !!!!!!
rendering::mesh g_TO_REMOVE_GOLEM_MESH;
target_handle G_TO_REMOVE_GOLEM_HANDLE;

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

    g_TO_REMOVE_GOLEM_MESH = rendering::make_cube(rendering::chunk_renderer::SQUARE_SIZE, glm::vec3{1.f, 0.f, 0.f});

    // Setup controls
    setup_inputs();

    // Setup units flyweights
    load_flyweights();

    G_TO_REMOVE_GOLEM_HANDLE = units.add(std::make_unique<unit>(glm::vec3{0.f, 0.f, 0.f}, glm::vec2{0.f, 0.f}, &unit_flyweights[106], &units));

    // Setup world rendering
    for(int x = 0; x < 20; ++x) {
        for(int z = 0; z < 20; ++z) {
            world_rendering.show(x, z);
        }
    }

    // Setup camera
    game_camera.reset({-100.f, 10.f, -200.f});

    // Setup mesh rendering
    setup_renderer();
}

void game::update(frame_duration last_frame_duration) {
    std::chrono::milliseconds last_frame_ms = std::chrono::duration_cast<std::chrono::milliseconds>(last_frame_duration);

    key_inputs.dispatch();

    unit* my_golem = static_cast<unit*>(G_TO_REMOVE_GOLEM_HANDLE.get());
    glm::vec3& golem_pos = my_golem->get_position();

    golem_pos.x += 0.1f * last_frame_ms.count();
    golem_pos.z += 0.1f * last_frame_ms.count();

	glm::vec3 position, dir;
	game_camera.screen_to_world_raw({300, 400}, 600, 800, position, dir);

	//std::cout << "Position  : " << "X :" << position.x << " Y :" << position.y << " Y :" << position.z << std::endl;
	//std::cout << "Direction  : " << "X :" << dir.x << " Y :" << dir.y << " Y :" << dir.z << std::endl;
}

void game::render() {
    world_rendering.render(mesh_rendering);

    // TODO: Render every units
    for(auto unit = units.begin_of_units(); unit != units.end_of_units(); ++unit) {
        rendering::mesh_renderer renderer(&g_TO_REMOVE_GOLEM_MESH, glm::translate(glm::mat4{1.f}, unit->second->get_position()), TEXTURE_NONE, PROGRAM_STANDARD);
        mesh_rendering.push(std::move(renderer));
    }

    // Render every meshes
    mesh_rendering.render();

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
        else if(event.button.button == SDL_BUTTON_LEFT) {
            glm::vec3 world_pos;
            glm::vec3 world_direction;

            std::cout << event.button.x << ", " << event.button.y << std::endl;
            game_camera.screen_to_world_raw(glm::vec2{event.button.x, event.button.y}, G_TO_REMOVE_SCREEN_WIDTH, G_TO_REMOVE_SCREEN_HEIGHT, world_pos, world_direction);

            units.add(std::make_unique<unit>(world_direction * game_camera.position().y + world_pos, glm::vec2{0.f, 0.f}, &unit_flyweights[106], &units));
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
        profiler_us p("key events");
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

    G_TO_REMOVE_SCREEN_WIDTH = new_width;
    G_TO_REMOVE_SCREEN_HEIGHT = new_height;
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