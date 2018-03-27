#include "game.hpp"
#include "control/all_commands.hpp"
#include "debug/profiler.hpp"
#include "constant/rendering.hpp"

#include "../common/actor/unit.hpp"
#include "../common/world/world_generator.hpp"
#include "../common/collision/collision_detector.hpp"
#include "../common/collision/aabb_shape.hpp"
#include "../common/datadriven/virtual_texture_list_record.hpp"
#include "../common/datadriven/shader_list_record.hpp"
#include "../common/datadriven/texture_list_record.hpp"
#include "../common/datadriven/data_list.hpp"
#include "../common/networking/packet.hpp"
#include "../common/networking/world_map.hpp"
#include "../common/networking/world_chunk.hpp"
#include "../common/networking/networking_constant.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <memory>
#include <functional>

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

void game::load_flyweights() {

    for(auto flyweight_iterator = std::begin(unit_flyweights()); flyweight_iterator != std::end(unit_flyweights()); ++flyweight_iterator) {
        const float half_width = flyweight_iterator->second.width() / 2.f;
        const float height = flyweight_iterator->second.height();
        auto it_value = virtual_textures.find(flyweight_iterator->second.texture());

        rendering::virtual_texture::area_type area;
        if(it_value != std::end(virtual_textures)) {
            area = it_value->second.area;
        }
        else {
            area = rendering::virtual_texture::area_type(0.f, 0.f, 1.f, 1.f);
        }

        rendering::mesh_builder builder;
        builder.add_vertex(glm::vec3{-half_width, 0.f,    0.f}, glm::vec2{area.left(),  area.top()});
        builder.add_vertex(glm::vec3{ half_width, 0.f,    0.f}, glm::vec2{area.right(), area.top()});
        builder.add_vertex(glm::vec3{ half_width, height, 0.f}, glm::vec2{area.right(), area.bottom()});

        builder.add_vertex(glm::vec3{-half_width, 0.f,    0.f}, glm::vec2{area.left(),  area.top()});
        builder.add_vertex(glm::vec3{ half_width, height, 0.f}, glm::vec2{area.right(), area.bottom()});
        builder.add_vertex(glm::vec3{-half_width, height, 0.f}, glm::vec2{area.left(),  area.bottom()});

        unit_meshes[flyweight_iterator->first] = builder.build();
    }
}

void game::setup_renderer() {
    mesh_rendering.set_camera(&game_camera);

    load_shaders();
    load_textures();
    load_virtual_textures();
}

void game::load_textures() {
    std::ifstream texture_list_stream("asset/data/texture.list");
    data::load_data_list<data::texture_list_record>(texture_list_stream, [this](const data::texture_list_record& record) {
        if(record.path != "NONE") {
            std::string fullpath = "asset/texture/" + record.path;
            gl::texture texture = gl::texture::load_from_path(fullpath.c_str());

            if (texture.good()) {
                textures[record.id] = std::move(texture);
            }
            else {
                std::cerr << "cannot load texture " << record.path << std::endl;
            }
        }
        else {
            textures[record.id] = gl::texture{};
        }
    });
}

void game::load_virtual_textures() {
    std::ifstream texture_list_stream("asset/data/virtual_texture.list");
    data::load_data_list<data::virtual_texture_list_record>(texture_list_stream, [this](const data::virtual_texture_list_record& record) {
        auto texture_it = textures.find(record.texture_id);

        if(texture_it != textures.end()) {
            mesh_rendering.set_texture(record.id, rendering::virtual_texture(texture_it->second, record.area));
            virtual_texture_value value;
            value.id = record.id;
            value.area = record.area;
            virtual_textures[record.name] = value;
        }
        else {
            std::cerr << "can't load virtual texture '" << record.name << "'" << std::endl;
        }
    });
}

void game::load_shaders() {
    std::ifstream program_list_stream("asset/data/shader.list");
    data::load_data_list<data::shader_list_record>(program_list_stream, [this](const data::shader_list_record& record) {
        auto vertex_shader = load_shader<gl::vertex_shader>("asset/shader/" + record.vertex_path);
        auto fragment_shader = load_shader<gl::fragment_shader>("asset/shader/" + record.fragment_path);

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

    // Change Unit To Spawn	using change_unit_command
    key_inputs.register_action(SDLK_1, KMOD_NONE, std::make_unique<input::change_unit_command>(next_unit_to_spawn, 100));
    key_inputs.register_action(SDLK_2, KMOD_NONE, std::make_unique<input::change_unit_command>(next_unit_to_spawn, 101));
    key_inputs.register_action(SDLK_3, KMOD_NONE, std::make_unique<input::change_unit_command>(next_unit_to_spawn, 102));
    key_inputs.register_action(SDLK_4, KMOD_NONE, std::make_unique<input::change_unit_command>(next_unit_to_spawn, 103));
    key_inputs.register_action(SDLK_5, KMOD_NONE, std::make_unique<input::change_unit_command>(next_unit_to_spawn, 104));
    key_inputs.register_action(SDLK_6, KMOD_NONE, std::make_unique<input::change_unit_command>(next_unit_to_spawn, 105));
    key_inputs.register_action(SDLK_7, KMOD_NONE, std::make_unique<input::change_unit_command>(next_unit_to_spawn, 106));
    key_inputs.register_action(SDLK_8, KMOD_NONE, std::make_unique<input::change_unit_command>(next_unit_to_spawn, 107));
    key_inputs.register_action(SDLK_9, KMOD_NONE, std::make_unique<input::change_unit_command>(next_unit_to_spawn, 108));
    key_inputs.register_action(SDLK_0, KMOD_NONE, std::make_unique<input::change_unit_command>(next_unit_to_spawn, 109));
    key_inputs.register_action(SDLK_p, KMOD_NONE, std::make_unique<input::change_unit_command>(next_unit_to_spawn, 110));
    key_inputs.register_action(SDLK_o, KMOD_NONE, std::make_unique<input::change_unit_command>(next_unit_to_spawn, 111));

    // Change Unit To Spawn	using generic_command
    key_inputs.register_action(SDLK_i, KMOD_NONE, input::make_generic_command([&]() { next_unit_to_spawn = 112; }));
}

// TODO: Move out of here
// Cette fonction indique si un unité peut se déplacer à une position donnée
bool game::can_move(base_unit* unit, glm::vec3 position) const {
    const float CHUNK_WIDTH = world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE;
    const float CHUNK_DEPTH = world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE;

    if(unit) {
        int chunk_x = static_cast<int>(position.x / CHUNK_WIDTH);
        int chunk_z = static_cast<int>(position.z / CHUNK_DEPTH);

        const world_chunk* chunk = game_world.chunk_at(chunk_x, chunk_z);
        if(chunk) {
            //const glm::vec3 chunk_space_position(position.x - chunk_x * CHUNK_WIDTH,
            //                                     position.y,
            //                                     position.z - chunk_z * CHUNK_DEPTH);

            //const int x_region = chunk_space_position.x / rendering::chunk_renderer::SQUARE_SIZE;
            //const int z_region = chunk_space_position.z / rendering::chunk_renderer::SQUARE_SIZE;

            //int region = chunk->biome_at(x_region, 0, z_region);

            return true;//region != BIOME_WATER;
        }
    }

    return false;
}

void game::load_datas() {
    // Setup mesh rendering
    setup_renderer();

    // Setup units flyweights
    load_flyweights();
}

game::game(networking::network_manager& manager, networking::network_manager::socket_handle socket)
: base_game(std::thread::hardware_concurrency() - 1, std::make_unique<unit_manager>())
, is_scrolling(false)
, game_world()
, world_rendering(game_world)
, game_camera(-400.f, 400.f, -400.f, 400.f, -1000.f, 1000.f)
, last_fps_duration_index(0)
, frame_count(0) 
, network{manager}
, socket(socket) {
    last_fps_durations.reserve(10);
}

void game::on_init() {
    auto flyweights_packet = network.wait_packet_from(PACKET_SETUP_FLYWEIGHTS, socket);

    if(flyweights_packet.first) {
        auto manager_u = flyweights_packet.second.as<std::unordered_map<std::string, unit_flyweight>>();
        unit_flyweight_manager manager;
        for (auto& v : manager_u)
        {
            manager.emplace(std::stoi(v.first), std::move(v.second));
        }
        set_flyweight_manager(manager);

        auto chunks_packet = network.wait_packet_from(PACKET_SETUP_CHUNK, socket);
        if(chunks_packet.first) {
            auto chunks = chunks_packet.second.as<std::vector<networking::world_chunk>>();

            for(networking::world_chunk& received_chunk : chunks) {
                world_chunk& game_chunk = game_world.add(received_chunk.x, received_chunk.y);
                game_chunk.set_biome_at(received_chunk.regions_biome);
            }
        }
        else {
            throw std::runtime_error("failed to load chunks");
        }

    }
    else {
        throw std::runtime_error("failed to load flyweights");
    }

    // Setup controls
    setup_inputs();

    // Setup world rendering
    for(int x = 0; x < 20; ++x) {
        for(int z = 0; z < 20; ++z) {
            world_rendering.show(x, z);
        }
    }

    // Setup camera
    game_camera.reset({-100.f, 10.f, -200.f});

    load_datas();
}

void game::on_release() {

}

void game::on_update(frame_duration last_frame_duration) {
    auto p = network.poll_packet_from(PACKET_SPAWN_UNITS, socket);
    if(p.first) {
        std::vector<unit> units = p.second.as<std::vector<unit>>();
        for(const unit& u : units) {
            add_unit(u.get_id(), u.get_position(), u.get_target_position(), u.get_type_id());
        }
    }

    std::chrono::milliseconds last_frame_ms = std::chrono::duration_cast<std::chrono::milliseconds>(last_frame_duration);
    auto update_p = network.poll_packet_from(PACKET_UPDATE_UNITS, socket);
    if(update_p.first) {
        std::vector<unit> units = update_p.second.as<std::vector<unit>>();
        for(const unit& u : units) {
            base_unit* my_unit = this->units().get(u.get_id());

            if(my_unit) {
                my_unit->set_position(u.get_position());
            }
        }
    }

    key_inputs.dispatch();

    //update_task.wait();
}

void game::render() {
    world_rendering.render(mesh_rendering);

    // TODO: Render every units
    for(auto unit = units().begin_of_units(); unit != units().end_of_units(); ++unit) {
        rendering::mesh_renderer renderer(&unit_meshes[unit->second->get_type_id()],
                                          glm::translate(glm::mat4{1.f}, unit->second->get_position()),
                                          virtual_textures[unit->second->texture()].id , PROGRAM_BILLBOARD);
        mesh_rendering.push(std::move(renderer));
    }

    // Render every meshes
    mesh_rendering.render();

    // Calculates FPS
    ++frame_count;
    if(fps_clock.elapsed_time<std::chrono::seconds>() >= std::chrono::seconds(1)) {
        fps_clock.substract(std::chrono::seconds(1)); // Keep time accumulation

        if(last_fps_durations.size() < last_fps_durations.capacity()) {
            last_fps_durations.push_back(frame_count);
            last_fps_duration_index = last_fps_durations.size() - 1;
        }
        else {
            last_fps_durations[last_fps_duration_index] = frame_count;
            last_fps_duration_index = (last_fps_duration_index + 1) % last_fps_durations.capacity();
        }

        frame_count = 0;
    }
}

bool inside_world_bound(glm::vec3 position) {
    const float CHUNK_WIDTH = world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE;
    const float CHUNK_DEPTH = world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE;

    float chunk_x = position.x / CHUNK_WIDTH;
    float chunk_z = position.z / CHUNK_DEPTH;

    if (chunk_x >= 20.f || chunk_z >= 20.f || chunk_x < 0.f || chunk_z < 0.f)
        return false;

    return true;
}

void game::handle_event(SDL_Event event) {
    if(event.type == SDL_MOUSEBUTTONDOWN) {
        if(event.button.button == SDL_BUTTON_MIDDLE) {
            is_scrolling = true;
        }
        else if(event.button.button == SDL_BUTTON_LEFT) {
            const float screen_half_width = G_TO_REMOVE_SCREEN_WIDTH / 2.f;
            const float screen_half_height = G_TO_REMOVE_SCREEN_HEIGHT / 2.f;

            const glm::vec2 coords{ event.button.x, G_TO_REMOVE_SCREEN_HEIGHT - event.button.y };
            const glm::vec2 normalized_coords{ (coords.x - screen_half_width) / screen_half_width, (coords.y - screen_half_height) / screen_half_height };

            glm::vec3 test = game_camera.world_coordinate_of(normalized_coords, { 0,0,0 }, {0,1,0});

            // clicked outside the map
            if (inside_world_bound(test))
            {
                //add_unit(test, glm::vec2{0.f, 0.f}, next_unit_to_spawn);

                for (auto u = units().begin_of_units(); u != units().end_of_units(); u++)
                {
                    unit* actual_unit = static_cast<unit*>(u->second.get());
                    actual_unit->set_target_position({ test.x, test.z });
                }
            }
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
        game_camera.adjust(-400.f * aspect, 400.f * aspect, -400.f, 400.f, -1000.f, 1000.f);
    }
    else {
        game_camera.adjust(-400.f, 400.f, -400.f / aspect, 400.f / aspect, -1000.f, 1000.f);
    }

    G_TO_REMOVE_SCREEN_WIDTH = new_width;
    G_TO_REMOVE_SCREEN_HEIGHT = new_height;
}

int game::fps() const noexcept {
    if(last_fps_durations.empty()) return 0;

    return last_fps_durations[last_fps_duration_index];
}

float game::average_fps() const noexcept {
    if(last_fps_durations.empty()) return 0.f;

    float sum = std::accumulate(std::begin(last_fps_durations), std::end(last_fps_durations), 0.f);
    return sum / last_fps_durations.size();
}