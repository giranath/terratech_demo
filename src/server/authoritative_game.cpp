#include "authoritative_game.hpp"
#include "../common/actor/unit.hpp"

#include <thread>
#include <iostream>

authoritative_game::authoritative_game()
: base_game(std::thread::hardware_concurrency() - 1) {

}

void authoritative_game::on_init() {

}

void authoritative_game::on_update(frame_duration last_frame) {
    std::chrono::milliseconds last_frame_ms = std::chrono::duration_cast<std::chrono::milliseconds>(last_frame);

    auto update_task = push_task(async::make_task([this, last_frame_ms]() {
        for (auto u = units().begin_of_units(); u != units().end_of_units(); u++)
        {
            unit* actual_unit = static_cast<unit*>(u->second.get());

            glm::vec2 target = actual_unit->get_target_position();
            glm::vec3 target3D = { target.x, 0, target.y };

            glm::vec3 direction = target3D - actual_unit->get_position();

            if (direction == glm::vec3{})
                continue;

            direction = glm::normalize(direction);

            glm::vec3 move = actual_unit->get_position() + (direction * 100.0f * (last_frame_ms.count() / 1000.0f));

            actual_unit->set_position(move);
        }
    }));

    update_task.wait();
}

void authoritative_game::on_release() {

}