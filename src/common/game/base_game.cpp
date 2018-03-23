#include "base_game.hpp"
#include "../actor/unit.hpp"

#include <iostream>

namespace gameplay {

base_game::base_game(memory::stack_allocator& allocator,
                     std::size_t thread_count,
                     std::unique_ptr<unit_manager> units)
: memory(allocator)
, managed_heap_memory(allocator.allocate(MANAGED_HEAP_SIZE))
, managed_heap(managed_heap_memory, MANAGED_HEAP_SIZE)
, managed_heap_allocator(managed_heap)
, allocation_reservations(managed_heap_allocator)
, tasks(thread_count)
, units_(std::move(units))
, will_loop(true) {

}

base_game::~base_game() {
    // Free game services
    for(auto it = std::rbegin(allocation_reservations); it != std::rend(allocation_reservations); ++it) {
        memory.free(it->first, it->second);
    }

    // Free the managed heap memory
    memory.free(managed_heap_memory, MANAGED_HEAP_SIZE);
}

void base_game::init() {
    on_init();
}

void base_game::release() {
    on_release();
}

base_game::unit_flyweight_manager& base_game::unit_flyweights() {
    return unit_flyweights_;
}

const base_game::unit_flyweight_manager& base_game::unit_flyweights() const {
    return unit_flyweights_;
}

unit_manager& base_game::units() {
    return *units_;
}

const unit_manager& base_game::units() const {
    return *units_;
}

bool base_game::is_running() const noexcept {
    return will_loop;
}

void base_game::update(frame_duration last_frame) {
    on_update(last_frame);
}

void base_game::stop() noexcept {
    on_stop();
    will_loop = false;
}

async::task_executor::task_future base_game::push_task(async::task_executor::task_ptr task) {
    return tasks.push(std::move(task));
}

target_handle base_game::add_unit(uint32_t id, glm::vec3 position, glm::vec2 target, int flyweight_id) {
    return units_->add(make_unit(position, target, flyweight_id), id);
}

unit_manager::unit_ptr base_game::make_unit(glm::vec3 position, glm::vec2 target, int flyweight_id) {
    return std::make_unique<unit>(position, target, &unit_flyweights_[flyweight_id], units_.get());
}

void base_game::load_flyweight(const nlohmann::json& json) {
    int id = json["id"];

    unit_flyweights_[id] = unit_flyweight(json);
}

void base_game::set_flyweight_manager(const unit_flyweight_manager& manager) {
    unit_flyweights_ = manager;
}

void base_game::set_flyweight_manager(unit_flyweight_manager&& manager) {
    unit_flyweights_ = manager;
}

memory::raw_memory_ptr base_game::reserve_memory_space(std::size_t size) {
    memory::raw_memory_ptr reserved_space = memory.allocate(size);

    if(reserved_space) {
        allocation_reservations.emplace_back(reserved_space, size);
    }

    return reserved_space;
}

memory::heap_allocator& base_game::heap_allocator() {
    return managed_heap;
}

}