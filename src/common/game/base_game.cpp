#include "base_game.hpp"
#include "../actor/unit.hpp"
#include "../world/world.hpp"

namespace gameplay {

bool base_game::can_move(base_unit* unit, glm::vec3 position, world& w) const {
    if(unit) {
        int chunk_x = static_cast<int>(position.x / world::CHUNK_WIDTH);
        int chunk_z = static_cast<int>(position.z / world::CHUNK_DEPTH);

        const world_chunk* chunk = w.chunk_at(chunk_x, chunk_z);
        if(chunk) {
            const glm::vec3 chunk_space_position(position.x - chunk_x * world::CHUNK_WIDTH,
                                                 position.y,
                                                 position.z - chunk_z * world::CHUNK_DEPTH);

            return chunk->biome_at(chunk_space_position.x, 0, chunk_space_position.z) != BIOME_WATER;
        }
    }

    return false;
}

base_game::base_game(std::size_t thread_count, std::unique_ptr<unit_manager> units)
: tasks(thread_count)
, units_(std::move(units))
, will_loop(true) {

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

unit_manager::unit_ptr base_game::make_unit(glm::vec3 position, int flyweight_id) {
	return std::make_unique<unit>(position, position, &unit_flyweights_[flyweight_id], units_.get());
}

void base_game::load_flyweight(const nlohmann::json& json) {
    int id = json["id"];

    unit_flyweights_[id] = unit_flyweight(json);
}

void base_game::set_flyweight_manager(const unit_flyweight_manager& manager)
{
    unit_flyweights_ = manager;
}
void base_game::set_flyweight_manager(unit_flyweight_manager&& manager)
{
    unit_flyweights_ = manager;
}
}