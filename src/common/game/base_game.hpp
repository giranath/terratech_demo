#ifndef MMAP_DEMO_BASE_GAME_HPP
#define MMAP_DEMO_BASE_GAME_HPP

#include "../actor/unit_flyweight.hpp"
#include "../async/task_executor.hpp"
#include "../actor/unit_manager.hpp"
#include "../time/clock.hpp"
#include "../memory/stack_allocator.hpp"
#include "../memory/heap_allocator.hpp"
#include "../memory/utils.hpp"
#include "../memory/allocator.hpp"

#include <mutex>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <vector>

namespace gameplay {

class base_game {
public:
    using clock = std::chrono::high_resolution_clock;
    using frame_duration = clock::duration;
    using unit_flyweight_manager = std::unordered_map<int, unit_flyweight,
                                                      std::hash<int>, std::equal_to<int>,
                                                      memory::container_heap_allocator<std::pair<const int, unit_flyweight>>>;
private:
    static const std::size_t MANAGED_HEAP_SIZE = memory::gigabits(2);

    memory::stack_allocator& memory;
    memory::raw_memory_ptr managed_heap_memory;
    memory::heap_allocator managed_heap;
    std::mutex stack_allocator_mutex;
    std::mutex heap_allocator_mutex;

    // Thread pool
    async::task_executor tasks;

    // Units
    std::unique_ptr<unit_manager> units_; // TODO: Custom allocator

    unit_flyweight_manager unit_flyweights_;

    // Game loop management
    bool will_loop;

protected:
    virtual void on_init() = 0;
    virtual void on_update(frame_duration last_frame) = 0;
    virtual void on_release() = 0;
    virtual void on_stop() {};

public:
    explicit base_game(memory::stack_allocator& allocator,
                       std::size_t thread_count,
                       std::unique_ptr<unit_manager> units);
    virtual ~base_game();

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
    target_handle add_unit(uint32_t id, glm::vec3 position, glm::vec2 target, int flyweight_id);
    unit_manager::unit_ptr make_unit(glm::vec3 position, glm::vec2 target, int flyweight_id); // TODO: Make const

    void load_flyweight(const nlohmann::json& json);

    void set_flyweight_manager(const unit_flyweight_manager& manager);
    void set_flyweight_manager(unit_flyweight_manager&& manager);

    memory::raw_memory_ptr reserve_memory_space(std::size_t size, uint16_t tag = 0);
    memory::raw_memory_ptr allocate_on_heap(std::size_t size, uint16_t tag = 0);
    void free_from_heap(memory::raw_memory_ptr ptr, std::size_t size);

    memory::heap_allocator& heap_allocator();
};

}

#endif //MMAP_DEMO_BASE_GAME_HPP
