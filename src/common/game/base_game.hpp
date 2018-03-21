#ifndef MMAP_DEMO_BASE_GAME_HPP
#define MMAP_DEMO_BASE_GAME_HPP

#include "../actor/unit_flyweight.hpp"
#include "../async/task_executor.hpp"
#include "../actor/unit_manager.hpp"
#include "../time/clock.hpp"

#include <chrono>
#include <unordered_map>

namespace gameplay {

class base_game {
public:
    using clock = std::chrono::high_resolution_clock;
    using frame_duration = clock::duration;
    using unit_flyweight_manager = std::unordered_map<int, unit_flyweight>;
private:
    // Thread pool
    async::task_executor tasks;

    // Units
    unit_manager units_;
    unit_flyweight_manager unit_flyweights_;

    // Game loop management
    bool will_loop;

protected:
    virtual void on_init() = 0;
    virtual void on_update(frame_duration last_frame) = 0;
    virtual void on_release() = 0;

public:
    explicit base_game(std::size_t thread_count);

    void init();
    void release();

    unit_flyweight_manager& unit_flyweights();
    const unit_flyweight_manager& unit_flyweights() const;

    unit_manager& units();
    const unit_manager& units() const;

    bool is_running() const noexcept;

    void update(frame_duration last_frame);

    void stop() noexcept;

    async::task_executor::task_future push_task(async::task_executor::task_ptr task);
    target_handle add_unit(glm::vec3 position, glm::vec2 target, int flyweight_id);

    void load_flyweight(const nlohmann::json& json);
};

}

#endif //MMAP_DEMO_BASE_GAME_HPP
