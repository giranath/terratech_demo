#include "authoritative_game.hpp"
#include "../common/actor/unit.hpp"
#include "../common/datadriven/data_list.hpp"

#include <thread>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

authoritative_game::authoritative_game()
: base_game(std::thread::hardware_concurrency() - 1)
, world(static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count())){

}

void authoritative_game::load_flyweights() {
    std::ifstream units_list_stream("asset/data/unit.list");
    if(!units_list_stream.is_open()) {
        std::cerr << "cannot open 'asset/data/unit.list'" << std::endl;
    }

    data::load_data_list<std::string>(units_list_stream, [this](const std::string& rel_path) {
        std::string full_path = "asset/data/" + rel_path;

        std::ifstream unit_stream(full_path);
        if(unit_stream.is_open()) {
            std::cout << "  loading flyweight '" << full_path << "'" << std::endl;
            load_flyweight(nlohmann::json::parse(unit_stream));
        }
        else {
            std::cerr << "  cannot open " << full_path << std::endl;
        }
    });
}

void authoritative_game::load_assets() {
    std::cout << "loading assets..." << std::endl;
    load_flyweights();
    // TODO: Load building
    // TODO: Load world generation
}

void authoritative_game::generate_world() {
    std::cout << "generating world..." << std::endl;
    for(std::size_t x = 0; x < 20; ++x) {
        for(std::size_t z = 0; z < 20; ++z) {
            world.generate_at(x, z);
        }
    }
}

void authoritative_game::on_init() {
    load_assets();
    generate_world();

    // TODO: Wait for connections
}

void authoritative_game::on_update(frame_duration last_frame) {
    std::chrono::milliseconds last_frame_ms = std::chrono::duration_cast<std::chrono::milliseconds>(last_frame);

    auto update_task = push_task(async::make_task([this, last_frame_ms]() {
        for (auto u = units().begin_of_units(); u != units().end_of_units(); u++) {
            unit* actual_unit = static_cast<unit*>(u->second.get());

            glm::vec2 target = actual_unit->get_target_position();
            glm::vec3 target3D = { target.x, 0, target.y };

            glm::vec3 direction = target3D - actual_unit->get_position();

            if (direction == glm::vec3{}) continue;

            direction = glm::normalize(direction);

            glm::vec3 move = actual_unit->get_position() + (direction * 100.0f * (last_frame_ms.count() / 1000.0f));

            actual_unit->set_position(move);
        }
    }));

    update_task.wait();
}

void authoritative_game::on_release() {

}