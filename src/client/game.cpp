#include "game.hpp"
#include "control/all_commands.hpp"
#include "control/build_commands.hpp"
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

        rendering::static_mesh_builder<6> builder;
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

bool inside_world_bound(glm::vec3 position) {
    const float CHUNK_WIDTH = world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE;
    const float CHUNK_DEPTH = world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE;

    float chunk_x = position.x / CHUNK_WIDTH;
    float chunk_z = position.z / CHUNK_DEPTH;

    if (chunk_x >= 20.f || chunk_z >= 20.f || chunk_x < 0.f || chunk_z < 0.f)
        return false;

    return true;
}


void game::setup_inputs() {
    input::event_manager::context& root_context = inputs.get(input::event_manager::root);

    // Camera movements
    root_context.register_key_state(SDLK_LEFT, std::make_unique<input::look_left_command>(game_camera, 10.f));
    root_context.register_key_state(SDLK_RIGHT, std::make_unique<input::look_right_command>(game_camera, 10.f));
    root_context.register_key_state(SDLK_UP, std::make_unique<input::look_up_command>(game_camera, 10.f));
    root_context.register_key_state(SDLK_DOWN, std::make_unique<input::look_down_command>(game_camera, 10.f));
    root_context.register_key_state(SDLK_a, std::make_unique<input::look_left_command>(game_camera, 10.f));
    root_context.register_key_state(SDLK_d, std::make_unique<input::look_right_command>(game_camera, 10.f));
    root_context.register_key_state(SDLK_w, std::make_unique<input::look_up_command>(game_camera, 10.f));
    root_context.register_key_state(SDLK_s, std::make_unique<input::look_down_command>(game_camera, 10.f));

    // Wireframe
    root_context.register_key_action(SDLK_m, KMOD_CTRL, std::make_unique<input::wireframe_command>());

    // Mouse scrolling
    root_context.register_mouse_drag(SDL_BUTTON_MIDDLE, [this](input::drag_event ev) {
        const glm::vec3 right_translation = game_camera.right() * ev.rel.x * -1.f;
        const glm::vec3 forward_translation = game_camera.forward() * ev.rel.y;
        const glm::vec3 cam_translation = right_translation + forward_translation;

        game_camera.translate(cam_translation);
    });

    // Unit selection
    root_context.register_mouse_click(SDL_BUTTON_LEFT, [this](input::click_event ev) {
        const float screen_half_width = G_TO_REMOVE_SCREEN_WIDTH / 2.f;
        const float screen_half_height = G_TO_REMOVE_SCREEN_HEIGHT / 2.f;

        const glm::vec2 coords{ ev.position.x, G_TO_REMOVE_SCREEN_HEIGHT - ev.position.y };
        const glm::vec2 normalized_coords{ (coords.x - screen_half_width) / screen_half_width, (coords.y - screen_half_height) / screen_half_height };

        glm::vec3 test = game_camera.world_coordinate_of(normalized_coords, { 0,0,0 }, {0,1,0});
        selected_unit_id = -1;

        // clicked outside the map
        if (inside_world_bound(test))
        {
            unit* clicked_unit = nullptr;
            int size = 0;
            units().units_in(glm::vec2(test.x / rendering::chunk_renderer::SQUARE_SIZE, test.z / rendering::chunk_renderer::SQUARE_SIZE),
                             &clicked_unit, [this, &size](unit* u) {
                        unit_id id(u->get_id());
                        if (id.player_id == player_id && size == 0)
                        {
                            ++size;
                            return true;
                        }
                        return false;
                    });
            if (clicked_unit)
            {
                selected_unit_id = clicked_unit->get_id();
                std::cout << "selected unit is " << selected_unit_id << std::endl;
            }
        }
    });

    root_context.register_mouse_click(SDL_BUTTON_RIGHT, [this](input::click_event ev) {
        const float screen_half_width = G_TO_REMOVE_SCREEN_WIDTH / 2.f;
        const float screen_half_height = G_TO_REMOVE_SCREEN_HEIGHT / 2.f;

        const glm::vec2 coords{ ev.position.x, G_TO_REMOVE_SCREEN_HEIGHT - ev.position.y };
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
                //TODO should remove
                std::vector<networking::update_target> updates;
                updates.emplace_back(selected_unit_id, glm::vec2(test.x / rendering::chunk_renderer::SQUARE_SIZE,
                                                                 test.z / rendering::chunk_renderer::SQUARE_SIZE));

                network.send_to(networking::packet::make(updates, PACKET_UPDATE_TARGETS), socket);
            }
        }
    });
}

void game::load_local_datas() {
    // Setup mesh rendering
    setup_renderer();

    // Setup units flyweights
    load_flyweights();
}

game::game(networking::network_manager& manager, networking::network_manager::socket_handle socket)
: base_game(std::thread::hardware_concurrency() - 1, std::make_unique<unit_manager>())
, game_world()
, world_rendering(game_world)
, game_camera(-400.f, 400.f, -400.f, 400.f, -1000.f, 1000.f)
, last_fps_duration_index(0)
, frame_count(0) 
, network{manager}
, socket(socket)
, selected_unit_id(-1)
, local_visibility(20 * world::CHUNK_WIDTH, 20 * world::CHUNK_DEPTH)
, fow_size(0) {
    last_fps_durations.reserve(10);
    discovered_chunks.reserve(20 * 20);
}

// TODO: Remove these
gl::vertex_array quad_VertexArrayID;
gl::buffer quad_vertexbuffer;

void game::setup_fog_of_war() {
    const std::size_t SQUARE_COUNT_PER_CHUNKS = world::CHUNK_WIDTH * world::CHUNK_DEPTH;
    const std::size_t VERTEX_COUNT_PER_SQUARE = 6;
    const std::size_t MAX_CHUNK_COUNT = 20;

    fow_vao = gl::vertex_array::make();
    gl::bind(fow_vao);

    // Setup Fog of War vertices buffer
    fow_vertices = gl::buffer::make();
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(fow_vertices));
    glBufferData(GL_ARRAY_BUFFER,
                 MAX_CHUNK_COUNT * SQUARE_COUNT_PER_CHUNKS * VERTEX_COUNT_PER_SQUARE * sizeof(glm::vec3),
                 NULL,
                 GL_DYNAMIC_DRAW);

    // Setup Fog of War colors buffer
    fow_colors = gl::buffer::make();
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(fow_colors));
    glBufferData(GL_ARRAY_BUFFER,
                 MAX_CHUNK_COUNT * SQUARE_COUNT_PER_CHUNKS * VERTEX_COUNT_PER_SQUARE * sizeof(glm::vec3),
                 NULL,
                 GL_DYNAMIC_DRAW);

}

void game::setup_screen_quad() {
    quad_VertexArrayID = gl::vertex_array::make();
    gl::bind(quad_VertexArrayID);
    static const GLfloat g_quad_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,
    };
    quad_vertexbuffer = gl::buffer::make();
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(quad_vertexbuffer));
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
}

void game::on_init() {
    // Setup disconnection handler
    network.on_disconnection.attach([this](const networking::network_manager::socket_handle disconnected_socket) {
        if(disconnected_socket == socket) {
            std::cout << "disconnected from server" << std::endl;
            stop();
        }
    });

    wait_for_server_init_datas();
    load_local_datas();

    setup_inputs();

    // Only show know chunks
    for (const world_chunk &known_chunks : game_world) {
        discovered_chunks.push_back(known_chunks.position());
    }

    // Setup camera
    game_camera.reset({-100.f, 10.f, -200.f});

    setup_fog_of_war();
    setup_screen_quad();
}

void game::wait_for_server_init_datas() {
    wait_for_player_id();
    wait_for_flyweights();
    wait_for_initial_chunks();
}

void game::wait_for_player_id() {
    auto client_informations = network.wait_packet_from(PACKET_PLAYER_ID, socket);
    if (client_informations.first) {
        auto infos = client_informations.second.as<networking::player_infos>();
        player_id = infos.id;
    }
    else {
        throw std::runtime_error("failed to receive player id");
    }
}

void game::wait_for_flyweights() {
    auto flyweights_packet = network.wait_packet_from(PACKET_SETUP_FLYWEIGHTS, socket);
    if (flyweights_packet.first) {
        auto manager_u = flyweights_packet.second.as<std::unordered_map<std::string, unit_flyweight>>();
        unit_flyweight_manager manager;
        for (auto &v : manager_u) {
            manager.emplace(std::stoi(v.first), std::move(v.second));
        }
        set_flyweight_manager(manager);
    }
    else {
        throw std::runtime_error("failed to load flyweights");
    }
}

void game::wait_for_initial_chunks() {
    auto chunks_packet = network.wait_packet_from(PACKET_SETUP_CHUNK, socket);
    if (chunks_packet.first) {
        auto chunks = chunks_packet.second.as<std::vector<networking::world_chunk>>();

        for (networking::world_chunk &received_chunk : chunks) {
            world_chunk &game_chunk = game_world.add(received_chunk.x, received_chunk.y);
            game_chunk.set_biome_at(received_chunk.regions_biome);

            for (const networking::resource &res : received_chunk.sites) {
                game_chunk.set_site_at(res.x, 0, res.y, site(res.type, res.quantity));
            }
        }
    }
    else {
        throw std::runtime_error("failed to load chunks");
    }
}

void game::on_release() {

}

void game::poll_server_changes() {
    poll_chunks_update();
    poll_units_update();
}

void game::poll_chunks_update() {
    auto p = network.poll_packet_from(PACKET_SETUP_CHUNK, socket);
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
}

void game::poll_units_update() {
    auto update_p = network.poll_packet_from(PACKET_UPDATE_UNITS, socket);
    if(update_p.first) {
        std::vector<unit> units = update_p.second.as<std::vector<unit>>();

        // TODO: Détecter unités qui ne sont plus à jour
        for(const unit& u : units) {
            unit_id id(u.get_id());
            unit* my_unit = static_cast<unit*>(this->units().get(u.get_id()));

            if(my_unit) {
                // Assumes that our units are correctly placed
                if(id.player_id != player_id) {
                    my_unit->set_position(u.get_position());
                    my_unit->set_target_position(u.get_target_position());
                }
            }
            else {
                add_unit(u.get_id(), u.get_position(), u.get_target_position(), u.get_type_id());
                game_camera.reset({ u.get_position().x * rendering::chunk_renderer::SQUARE_SIZE,
                                    game_camera.position().y,
                                    u.get_position().z * rendering::chunk_renderer::SQUARE_SIZE });
            }
        }
    }
}

void game::cull_out_of_view_chunks() {
    profiler_us cull_prof("cull chunks");
    const bounding_box<float> cam_view_box = camera_bounding_box();
    world_rendering.hide_all();
    std::for_each(std::begin(discovered_chunks), std::end(discovered_chunks), [this, &cam_view_box](const glm::i32vec2& pos) {
        const bounding_box<float> chunk_box(pos.x * world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE,
                                            pos.y * world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE,
                                            pos.x * world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE + world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE,
                                            pos.y * world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE + world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE);
        if(cam_view_box.intersect(chunk_box)) {
            profiler_us prof("show chunk");
            world_rendering.show(pos.x, pos.y);
        }
    });

}

void game::on_update(frame_duration last_frame_duration) {
    std::chrono::milliseconds last_frame_ms = std::chrono::duration_cast<std::chrono::milliseconds>(last_frame_duration);

    auto visibility_task = push_task(std::make_unique<task::update_player_visibility>(player_id, local_visibility, units()));

    poll_server_changes();

    auto update_task = push_task(std::make_unique<task::update_units>(units(), game_world, last_frame_ms.count() / 1000.0f));

    inputs.dispatch();

    update_task.wait();

    cull_out_of_view_chunks();

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
    //TODO reserve vertices
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> colors;

    std::for_each(std::begin(world_rendering), std::end(world_rendering), [this, &vertices, &colors](const rendering::world_renderer::chunk_rendering& chunk_renderer) {
        auto chunk_pos = chunk_renderer.pos;

        if(chunk_renderer.is_visible) {
            for(std::size_t x = 0; x < world::CHUNK_WIDTH; ++x) {
                for(std::size_t z = 0; z < world::CHUNK_DEPTH; ++z) {
                    const std::size_t world_pos_x = chunk_pos.x * world::CHUNK_WIDTH + x;
                    const std::size_t world_pos_z = chunk_pos.y * world::CHUNK_DEPTH + z;

                    const glm::vec3 start_of_square(world_pos_x * rendering::chunk_renderer::SQUARE_SIZE,
                                                    1.f,
                                                    world_pos_z * rendering::chunk_renderer::SQUARE_SIZE);
                    const glm::vec3 right_of_square(rendering::chunk_renderer::SQUARE_SIZE, 0.f, 0.f);
                    const glm::vec3 back_of_square(0.f, 0.f, rendering::chunk_renderer::SQUARE_SIZE);

                    glm::vec3 color;
                    bool hide_under;

                    switch(local_visibility.at(world_pos_x, world_pos_z)) {
                        case visibility::unexplored:
                            hide_under = false;
                            color = glm::vec3(0.f, 0.f, 0.f);
                            break;
                        case visibility::explored:
                            hide_under = true;
                            color = glm::vec3(0.5f, 0.5f, 0.5f);
                            break;
                        case visibility::visible:
                            hide_under = true;
                            color = glm::vec3(1.f, 1.f, 1.f);
                            break;
                    }

                    if(hide_under) {
                        vertices.push_back(start_of_square);
                        vertices.push_back(start_of_square + right_of_square);
                        vertices.push_back(start_of_square + right_of_square + back_of_square);
                        vertices.push_back(start_of_square);
                        vertices.push_back(start_of_square + right_of_square + back_of_square);
                        vertices.push_back(start_of_square + back_of_square);
                        for(int i = 0; i < 6; ++i) {
                            colors.push_back(color);
                        }
                    }
                }
            }
        }
    });

    // Update fog of war buffers
    fow_size = vertices.size();
    if(fow_size > 0) {
        gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(fow_vertices));
        glBufferSubData(GL_ARRAY_BUFFER, 0, fow_size * sizeof(glm::vec3), &vertices[0]);

        gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(fow_colors));
        glBufferSubData(GL_ARRAY_BUFFER, 0, fow_size * sizeof(glm::vec3), &colors[0]);
    }
}

void game::render_game_state() {
    gl::bind(gl::framebuffer_bind<>(fbo));
    glViewport(0, 0, G_TO_REMOVE_SCREEN_WIDTH, G_TO_REMOVE_SCREEN_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render_chunks();
    render_units();

    mesh_rendering.render();
}

void game::render_chunks() {
    profiler_us render_prof("render chunks");
    const bounding_box<float> cam_view_box = camera_bounding_box();
    world_rendering.hide_all();
    std::for_each(std::begin(discovered_chunks), std::end(discovered_chunks), [this, &cam_view_box](const glm::i32vec2& pos) {
        const bounding_box<float> chunk_box(pos.x * world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE,
                                            pos.y * world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE,
                                            pos.x * world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE + world::CHUNK_WIDTH * rendering::chunk_renderer::SQUARE_SIZE,
                                            pos.y * world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE + world::CHUNK_DEPTH * rendering::chunk_renderer::SQUARE_SIZE);
        if(cam_view_box.intersect(chunk_box)) {
            profiler_us prof("show chunk");
            world_rendering.show(pos.x, pos.y);
        }
    });

    world_rendering.render(mesh_rendering);
}

void game::render_units() {
    for(auto unit = units().begin_of_units(); unit != units().end_of_units(); ++unit) {
        if(unit->second->is_visible()) {
            rendering::mesh_renderer renderer(&unit_meshes[unit->second->get_type_id()],
                                              glm::translate(glm::mat4{1.f}, unit->second->get_position() *
                                                                             rendering::chunk_renderer::SQUARE_SIZE),
                                              virtual_textures[unit->second->texture()].id, PROGRAM_BILLBOARD);
            mesh_rendering.push(std::move(renderer));
        }
    }
}

void game::render_fog_of_war() {
    gl::bind(gl::framebuffer_bind<>(fow_fbo));
    glViewport(0, 0, G_TO_REMOVE_SCREEN_WIDTH, G_TO_REMOVE_SCREEN_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl::bind(fow_vao);
    gl::program* current_program = mesh_rendering.program(PROGRAM_FOW);
    gl::bind(*current_program);

    auto projection_uniform = current_program->find_uniform<glm::mat4>("projection_matrix");
    auto view_uniform = current_program->find_uniform<glm::mat4>("view_matrix");
    auto model_uniform = current_program->find_uniform<glm::mat4>("model_matrix");
    model_uniform.set(glm::mat4{1.f});

    projection_uniform.set(game_camera.projection());
    view_uniform.set(game_camera.view());

    glEnableVertexAttribArray(0);
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(fow_vertices));
    // TODO: Move up
    glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );
    glEnableVertexAttribArray(1);
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(fow_colors));
    glVertexAttribPointer(
            1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );

    glDrawArrays(GL_TRIANGLES, 0, fow_size);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}

void game::render_on_screen() {
    gl::bind(gl::framebuffer_bind<>(gl::frame_buffer::SCREEN));
    glViewport(0, 0, G_TO_REMOVE_SCREEN_WIDTH, G_TO_REMOVE_SCREEN_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl::bind(quad_VertexArrayID);
    gl::program* fullscreen_prog = mesh_rendering.program(2);
    auto texture_uniform = fullscreen_prog->find_uniform<int>("game_texture");
    auto texture_uniform2 = fullscreen_prog->find_uniform<int>("fow_texture");

    gl::bind(*fullscreen_prog);
    texture_uniform.set(0);
    texture_uniform2.set(1);

    glActiveTexture(GL_TEXTURE0);
    gl::bind(gl::texture_bind<GL_TEXTURE_2D>(game_color_texture));
    glActiveTexture(GL_TEXTURE1);
    gl::bind(gl::texture_bind<GL_TEXTURE_2D>(fow_color_texture));
    glEnableVertexAttribArray(0);
    gl::bind(gl::buffer_bind<GL_ARRAY_BUFFER>(quad_vertexbuffer));
    glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
}

void game::calculate_fps() {
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

void game::render() {
    render_game_state();
    render_fog_of_war();
    render_on_screen();

    calculate_fps();
}

void game::handle_event(SDL_Event event) {
    inputs.handle(event);
}

void game::resize(int new_width, int new_height) {
    fbo = gl::frame_buffer::make();
    {
        gl::bind(gl::framebuffer_bind<>(fbo));

        game_color_texture = gl::texture::make(new_width, new_height);
        game_depth_buffer = gl::render_buffer::make();
        {
            gl::bind(gl::renderbuffer_bind<GL_RENDERBUFFER>(game_depth_buffer));
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, new_width, new_height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, game_depth_buffer);
        }
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, game_color_texture, 0);
        GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, draw_buffers);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("failed to create Framebuffer");
        }
    }
    fow_fbo = gl::frame_buffer::make();
    {
        gl::bind(gl::framebuffer_bind<>(fow_fbo));

        fow_color_texture = gl::texture::make(new_width, new_height);
        fow_depth_buffer = gl::render_buffer::make();
        {
            gl::bind(gl::renderbuffer_bind<GL_RENDERBUFFER>(fow_depth_buffer));
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, new_width, new_height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fow_depth_buffer);
        }
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fow_color_texture, 0);
        GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, draw_buffers);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("failed to create Framebuffer");
        }
    }

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