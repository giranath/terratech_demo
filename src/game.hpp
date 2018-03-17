#ifndef MMAP_DEMO_GAME_HPP
#define MMAP_DEMO_GAME_HPP

#include "async/task_executor.hpp"
#include "opengl/opengl.hpp"
#include "world/world.hpp"
#include "rendering/world_renderer.hpp"
#include "rendering/camera.hpp"
#include "rendering/rendering_manager.hpp"
#include "sdl/sdl.hpp"
#include "control/key_input_handler.hpp"
#include "actor/unit_manager.hpp"

#include <chrono>
#include <array>
#include <unordered_map>

class game {
public:
    using clock = std::chrono::high_resolution_clock;
    using frame_duration = clock::duration;
    using unit_flyweight_manager = std::unordered_map<int, unit_flyweight>;
private:
    struct virtual_texture_value {
        int id;
        rendering::virtual_texture::area_type area;
    };
    async::task_executor tasks;
    input::key_input_handler key_inputs;
    std::unordered_map<int, gl::texture> textures;
    std::unordered_map<std::string, virtual_texture_value> virtual_textures;
    world game_world;
    unit_manager units;
    unit_flyweight_manager unit_flyweights;
    rendering::world_renderer world_rendering;
    rendering::mesh_rendering_system mesh_rendering;
    rendering::camera game_camera;
    bool is_scrolling;
    bool is_running;
    std::array<int, 10> last_fps_durations;
    std::size_t last_fps_duration_index;
    int frame_count;
    clock::time_point last_fps_timepoint;

    void load_flyweight(std::ifstream& stream);
    void load_flyweights();
    void setup_inputs();
    void setup_renderer();
    void load_textures();
    void load_virtual_textures();
    void load_shaders();

    bool can_move(base_unit* unit, glm::vec3 position) const;

public:
    game();

    void update(frame_duration last_frame_duration);

    void render();

    void handle_event(SDL_Event event);
    void resize(int new_width, int new_height);

    bool wants_to_die() const noexcept;
    void kill() noexcept;

    int fps() const noexcept;
};


#endif //MMAP_DEMO_GAME_HPP
