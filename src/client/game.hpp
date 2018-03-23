#ifndef MMAP_DEMO_GAME_HPP
#define MMAP_DEMO_GAME_HPP

#include "../common/async/task_executor.hpp"
#include "opengl/opengl.hpp"
#include "../common/world/world.hpp"
#include "rendering/world_renderer.hpp"
#include "rendering/camera.hpp"
#include "rendering/rendering_manager.hpp"
#include "sdl/sdl.hpp"
#include "control/key_input_handler.hpp"
#include "../common/actor/unit_manager.hpp"
#include "../common/time/clock.hpp"
#include "../common/game/base_game.hpp"
#include "../common/networking/tcp_socket.hpp"
#include "../common/networking/socket_set.hpp"

#include <chrono>
#include <array>
#include <map>

class game : public gameplay::base_game {
public:
    using unit_mesh_collection = std::map<int, rendering::mesh>;
private:
    struct virtual_texture_value {
        int id;
        rendering::virtual_texture::area_type area;
    };

    // Inputs
    input::key_input_handler key_inputs;
    bool is_scrolling;

    // Textures
    std::unordered_map<int, gl::texture> textures;
    std::unordered_map<std::string, virtual_texture_value> virtual_textures;

    // World
    world game_world;

    // Units
    unit_mesh_collection unit_meshes;

    // Rendering
    rendering::world_renderer world_rendering;
    rendering::mesh_rendering_system mesh_rendering;
    rendering::camera game_camera;

    // FPS
    std::vector<int> last_fps_durations;
    std::size_t last_fps_duration_index;
    int frame_count;
    game_time::highres_clock fps_clock;

    // Gameplay
    int next_unit_to_spawn = 106;
    bool can_move(base_unit* unit, glm::vec3 position) const;


    //Networking
    networking::tcp_socket& socket;
    networking::socket_set socket_s;

    // Initialization functions
    void load_flyweights();
    void setup_inputs();
    void setup_renderer();
    void load_textures();
    void load_virtual_textures();
    void load_shaders();

    void load_datas();

public:
    game(memory::stack_allocator& allocator, networking::tcp_socket& socket);
    
    void on_init() override;
    void on_update(frame_duration last_frame) override;
    void on_release() override;

    void render();

    void handle_event(SDL_Event event);
    void resize(int new_width, int new_height);

    int fps() const noexcept;
    float average_fps() const noexcept;
};


#endif //MMAP_DEMO_GAME_HPP