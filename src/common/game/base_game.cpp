#include "base_game.hpp"
#include "../actor/unit.hpp"

namespace gameplay {

base_game::base_game(std::size_t thread_count)
: tasks(thread_count)
, will_loop(true) {

}

base_game::unit_flyweight_manager& base_game::unit_flyweights() {
    return unit_flyweights_;
}

const base_game::unit_flyweight_manager& base_game::unit_flyweights() const {
    return unit_flyweights_;
}

unit_manager& base_game::units() {
    return units_;
}

const unit_manager& base_game::units() const {
    return units_;
}

bool base_game::is_running() const noexcept {
    return will_loop;
}

void base_game::update(frame_duration last_frame) {
    on_update(last_frame);
}

void base_game::stop() noexcept {
    will_loop = false;
}

async::task_executor::task_future base_game::push_task(async::task_executor::task_ptr task) {
    return tasks.push(std::move(task));
}

target_handle base_game::add_unit(glm::vec3 position, glm::vec2 target, int flyweight_id) {
    return units_.add(std::make_unique<unit>(position, target, &unit_flyweights_[flyweight_id], &units_));
}

}