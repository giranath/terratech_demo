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

#include <chrono>
#include <array>

class game {
public:
    using clock = std::chrono::high_resolution_clock;
    using frame_duration = clock::duration;
private:
    async::task_executor tasks;
    input::key_input_handler key_inputs;
    world game_world;
    world_renderer world_rendering;
    mesh_rendering_system mesh_rendering;
    camera game_camera;
    bool is_scrolling;
    bool is_running;
    std::array<int, 10> last_fps_durations;
    std::size_t last_fps_duration_index;
    int frame_count;
    clock::time_point last_fps_timepoint;
public:
    game();

    /**
     * Updates the game state
     * @param last_frame_duration As the name implies, it's the duration of the last processed frame
     */
    void update(frame_duration last_frame_duration);

    /**
     * Render the current state on screen
     */
    void render();

    void handle_event(SDL_Event event);
    void resize(int new_width, int new_height);

    bool wants_to_die() const noexcept;
    void kill() noexcept;

    int fps() const noexcept;
};


#endif //MMAP_DEMO_GAME_HPP
