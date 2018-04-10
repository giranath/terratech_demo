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
#include "../common/networking/update_target.hpp"
#include "../common/task/update_player_visibility.hpp"
#include "../common/task/update_units.hpp"
#include "../common/networking/player_init.hpp"

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
, socket(socket)
, selected_unit_id(-1)
, local_visibility(20 * world::CHUNK_WIDTH, 20 * world::CHUNK_DEPTH){
    last_fps_durations.reserve(10);

    discovered_chunks.reserve(20 * 20);
}

void game::on_init() {
    auto client_informations = network.wait_packet_from(PACKET_PLAYER_ID, socket);
    if(client_informations.first) {
        auto infos = client_informations.second.as<networking::player_infos>();
        player_id = infos.id;
    }

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

                for(const networking::resource& res : received_chunk.sites) {
                    game_chunk.set_site_at(res.x, 0, res.y, site(res.type, res.quantity));
                }
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

    // Only show know chunks
    for(const world_chunk& known_chunks : game_world) {
        discovered_chunks.push_back(known_chunks.position());
    }

    // Setup camera
    game_camera.reset({-100.f, 10.f, -200.f});

    load_datas();

    network.on_disconnection.attach([this](const networking::network_manager::socket_handle disconnected_socket) {
        if(disconnected_socket == socket) {
            std::cout << "disconnected from server" << std::endl;
            stop();
        }
    });
}

void game::on_release() {

}

void game::on_update(frame_duration last_frame_duration) {
    std::chrono::milliseconds last_frame_ms = std::chrono::duration_cast<std::chrono::milliseconds>(last_frame_duration);

    auto visibility_task = push_task(std::make_unique<task::update_player_visibility>(player_id, local_visibility, units()));

    // Spawn new units
    auto p = network.poll_packet_from(PACKET_SPAWN_UNITS, socket);
    if(p.first) {
        std::vector<unit> units = p.second.as<std::vector<unit>>();
        for(const unit& u : units) {
            add_unit(u.get_id(),
                     u.get_position(),
                     u.get_target_position(),
                     u.get_type_id());
        }

        glm::vec3 target_position = units.front().get_position();
        game_camera.reset({target_position.x * rendering::chunk_renderer::SQUARE_SIZE,
                           game_camera.position().y,
                           target_position.z * rendering::chunk_renderer::SQUARE_SIZE});
    }

    // Update chunks
    p = network.poll_packet_from(PACKET_SETUP_CHUNK, socket);
    if(p.first) {
        auto chunks = p.second.as<std::vector<networking::world_chunk>>();

        for(networking::world_chunk& received_chunk : chunks) {
            world_chunk& game_chunk = game_world.add(received_chunk.x, received_chunk.y);
            game_chunk.set_biome_at(received_chunk.regions_biome);

            for(const networking::resource& res : received_chunk.sites) {
                game_chunk.set_site_at(res.x, 0, res.y, site(res.type, res.quantity));
            }

            auto it = std::find(std::begin(discovered_chunks), std::end(discovered_chunks), glm::i32vec2(received_chunk.x, received_chunk.y));
            if(it == std::end(discovered_chunks)) {
                discovered_chunks.emplace_back(received_chunk.x, received_chunk.y);
            }
        }
    }

    // Update units
    auto update_p = network.poll_packet_from(PACKET_UPDATE_UNITS, socket);
    if(update_p.first) {
        std::vector<unit> units = update_p.second.as<std::vector<unit>>();
        for(const unit& u : units) {
            unit* my_unit = static_cast<unit*>(this->units().get(u.get_id()));
			
            if(my_unit) {
                my_unit->set_position(u.get_position());
                my_unit->set_target_position(u.get_target_position());
            }
			else {
				add_unit(u.get_id(), u.get_position(), u.get_target_position(), u.get_type_id());
				game_camera.reset({ u.get_position().x * rendering::chunk_renderer::SQUARE_SIZE,
					game_camera.position().y,
					u.get_position().z * rendering::chunk_renderer::SQUARE_SIZE });
			}
        }
    }

    auto update_task = push_task(std::make_unique<task::update_units>(units(), game_world, last_frame_ms.count() / 1000.0f));

    key_inputs.dispatch();

    update_task.wait();

    // Only show visibles chunks
    const bounding_box<float> cam_view_box = camera_bounding_box();
    world_rendering.hide_all();
    std::for_each(std::begin(discovered_chunks), std::end(discovered_chunks), [this, &cam_view_box](const glm::i32vec2& pos) {
        const bounding_box<float> chunk_box(pos.x * world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE,
                                            pos.y * world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE,
                                            pos.x * world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE + world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE,
                                            pos.y * world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE + world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE);
        if(cam_view_box.intersect(chunk_box)) {
            world_rendering.show(pos.x, pos.y);
        }
    });

    // Update fog of war
    auto visiblity_ptr = visibility_task.get();
    auto visibility_task_ptr = static_cast<task::update_player_visibility*>(visiblity_ptr.get());
    if(visibility_task_ptr) {
        local_visibility = visibility_task_ptr->visibility();

        // TODO: Only update if changed
        update_fog_of_war();
    }
}

bounding_box<float> game::camera_bounding_box() const noexcept {
    const glm::vec3 top_left = game_camera.world_coordinate_of(    glm::vec2{-1.f,  1.f}, { 0,0,0 }, {0,1,0});
    const glm::vec3 top_right = game_camera.world_coordinate_of(   glm::vec2{ 1.f,  1.f}, { 0,0,0 }, {0,1,0});
    const glm::vec3 bottom_left = game_camera.world_coordinate_of( glm::vec2{-1.f, -1.f}, { 0,0,0 }, {0,1,0});
    const glm::vec3 bottom_right = game_camera.world_coordinate_of(glm::vec2{ 1.f, -1.f}, { 0,0,0 }, {0,1,0});

    return bounding_box<float>(bottom_right.x, bottom_left.z, top_left.x, top_right.z);
}

void game::update_fog_of_war() {
    rendering::mesh_builder fow_builder;

    std::for_each(std::begin(world_rendering), std::end(world_rendering), [this, &fow_builder](const rendering::world_renderer::chunk_rendering& chunk_renderer) {
        auto chunk_pos = chunk_renderer.pos;

        for(std::size_t x = 0; x < world::CHUNK_WIDTH; ++x) {
            for(std::size_t z = 0; z < world::CHUNK_DEPTH; ++z) {
                const std::size_t world_pos_x = chunk_pos.x * world::CHUNK_WIDTH + x;
                const std::size_t world_pos_z = chunk_pos.y * world::CHUNK_DEPTH + z;

                const glm::vec3 start_of_square(world_pos_x * rendering::chunk_renderer::SQUARE_SIZE,
                                                10.f,
                                                world_pos_z * rendering::chunk_renderer::SQUARE_SIZE);
                const glm::vec3 right_of_square(rendering::chunk_renderer::SQUARE_SIZE, 0.f, 0.f);
                const glm::vec3 back_of_square(0.f, 0.f, rendering::chunk_renderer::SQUARE_SIZE);

                glm::vec3 color;
                bool hide_under;

                switch(local_visibility.at(world_pos_x, world_pos_z)) {
                    case visibility::unexplored:
                        hide_under = true;
                        color = glm::vec3(0.f, 0.f, 0.f);
                        break;
                    case visibility::explored:
                        hide_under = true;
                        color = glm::vec3(0.5f, 0.5f, 0.5f);
                        break;
                    case visibility::visible:
                        hide_under = false;
                        break;
                }

                if(hide_under) {
                    fow_builder.add_vertex(start_of_square,                                    glm::vec2{}, color);
                    fow_builder.add_vertex(start_of_square + right_of_square,                  glm::vec2{}, color);
                    fow_builder.add_vertex(start_of_square + right_of_square + back_of_square, glm::vec2{}, color);

                    fow_builder.add_vertex(start_of_square,                                    glm::vec2{}, color);
                    fow_builder.add_vertex(start_of_square + right_of_square + back_of_square, glm::vec2{}, color);
                    fow_builder.add_vertex(start_of_square + back_of_square,                   glm::vec2{}, color);
                }
            }
        }
    });

    fog_of_war = fow_builder.build();
}

void game::render() {
    // Render world
    const glm::vec3 top_left = game_camera.world_coordinate_of(    glm::vec2{-1.f,  1.f}, { 0,0,0 }, {0,1,0});
    const glm::vec3 top_right = game_camera.world_coordinate_of(   glm::vec2{ 1.f,  1.f}, { 0,0,0 }, {0,1,0});
    const glm::vec3 bottom_left = game_camera.world_coordinate_of( glm::vec2{-1.f, -1.f}, { 0,0,0 }, {0,1,0});
    const glm::vec3 bottom_right = game_camera.world_coordinate_of(glm::vec2{ 1.f, -1.f}, { 0,0,0 }, {0,1,0});

    const bounding_box<float> cam_view_box(bottom_right.x, bottom_left.z, top_left.x, top_right.z);
    world_rendering.hide_all();
    std::for_each(std::begin(discovered_chunks), std::end(discovered_chunks), [this, &cam_view_box](const glm::i32vec2& pos) {
        const bounding_box<float> chunk_box(pos.x * world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE,
                                            pos.y * world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE,
                                            pos.x * world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE + world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE,
                                            pos.y * world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE + world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE);
        if(cam_view_box.intersect(chunk_box)) {
            world_rendering.show(pos.x, pos.y);
        }
    });

    world_rendering.render(mesh_rendering);

    // Render every units
    for(auto unit = units().begin_of_units(); unit != units().end_of_units(); ++unit) {
        rendering::mesh_renderer renderer(&unit_meshes[unit->second->get_type_id()],
                                          glm::translate(glm::mat4{1.f}, unit->second->get_position() * rendering::chunk_renderer::SQUARE_SIZE),
                                          virtual_textures[unit->second->texture()].id , PROGRAM_BILLBOARD);
        mesh_rendering.push(std::move(renderer));
    }

    // Construct fog of war
    rendering::mesh_renderer fow_renderer(&fog_of_war, glm::mat4{1.f}, virtual_textures["fog_of_war"].id, PROGRAM_STANDARD);
    mesh_rendering.push(std::move(fow_renderer));

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
            selected_unit_id = -1;

            // clicked outside the map
            if (inside_world_bound(test))
            {
                auto clicked_units = units().units_in(glm::vec2(test.x / rendering::chunk_renderer::SQUARE_SIZE,
                                                                test.z / rendering::chunk_renderer::SQUARE_SIZE));

                if(!clicked_units.empty()) {
                    unit* clicked_unit = clicked_units.front();

                    selected_unit_id = clicked_unit->get_id();
                    std::cout << "selected unit is " << selected_unit_id << std::endl;
                }
            }
        }
        else if(event.button.button == SDL_BUTTON_RIGHT) {
            const float screen_half_width = G_TO_REMOVE_SCREEN_WIDTH / 2.f;
            const float screen_half_height = G_TO_REMOVE_SCREEN_HEIGHT / 2.f;

            const glm::vec2 coords{ event.button.x, G_TO_REMOVE_SCREEN_HEIGHT - event.button.y };
            const glm::vec2 normalized_coords{ (coords.x - screen_half_width) / screen_half_width, (coords.y - screen_half_height) / screen_half_height };

            glm::vec3 test = game_camera.world_coordinate_of(normalized_coords, { 0,0,0 }, {0,1,0});

            if(inside_world_bound(test) && selected_unit_id != -1) {
                // Update target of unit
                base_unit* selected_unit = units().get(selected_unit_id);
                if(selected_unit) {
                    unit* u = static_cast<unit*>(selected_unit);
                    u->set_target_position(glm::vec2(test.x / rendering::chunk_renderer::SQUARE_SIZE,
                                                     test.z / rendering::chunk_renderer::SQUARE_SIZE));

                    // Send to server
                    std::vector<networking::update_target> updates;
                    updates.emplace_back(selected_unit_id, glm::vec2(test.x / rendering::chunk_renderer::SQUARE_SIZE,
                                                                     test.z / rendering::chunk_renderer::SQUARE_SIZE));

                    network.send_to(networking::packet::make(updates, PACKET_UPDATE_TARGETS), socket);
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