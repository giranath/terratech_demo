#include "update_units.hpp"

namespace task {

bool update_units::can_move(base_unit* unit, glm::vec3 position, world& w) noexcept {
    if(unit) {
        int chunk_x = static_cast<int>(position.x / world::CHUNK_WIDTH);
        int chunk_z = static_cast<int>(position.z / world::CHUNK_DEPTH);

        const world_chunk* chunk = w.chunk_at(chunk_x, chunk_z);
        if(chunk) {
            const glm::vec3 chunk_space_position(position.x - chunk_x * world::CHUNK_WIDTH,
                                                 position.y,
                                                 position.z - chunk_z * world::CHUNK_DEPTH);

            const int biome = chunk->biome_at(chunk_space_position.x, 0, chunk_space_position.z);

            biome_type b;
            switch(biome) {
                case BIOME_WATER:
                    b = biome_type::water;
                    break;
                case BIOME_SNOW:
                    b = biome_type::snow;
                    break;
                case BIOME_DESERT:
                    b = biome_type::desert;
                    break;
                case BIOME_ROCK:
                    b = biome_type::rock;
                    break;
                case BIOME_GRASS:
                    b = biome_type::grass;
                    break;
                default:
                    b = biome_type::unknown;
                    break;
            }

            return unit->can_walk_on(b);
        }
    }

    return false;
}

update_units::update_units(unit_manager &units, world& w, float elapsed_seconds)
: units(units)
, w(w)
, elapsed_seconds(elapsed_seconds) {

}

void update_units::execute() {
    for (auto u = units.begin_of_units(); u != units.end_of_units(); u++) {
        auto actual_unit = u->second;

        const glm::vec2 target = actual_unit->get_target_position();
        const glm::vec3 target3D = { target.x, 0, target.y };
        const glm::vec3 displacement = target3D - actual_unit->get_position();
        const float len = glm::length(displacement);

        if(len > 0.f) {

            if(len < 0.1f) {
                actual_unit->set_position(target3D);
            }
            else {
                glm::vec3 direction = glm::normalize(displacement);

                glm::vec3 new_position =
                        actual_unit->get_position() + (direction * actual_unit->get_speed() * elapsed_seconds);

                if (can_move(actual_unit, new_position, w)) {
                    actual_unit->set_position(new_position);
                } else {
                    actual_unit->set_target_position(
                            glm::vec2(actual_unit->get_position().x, actual_unit->get_position().z));
                }
            }
        }
    }
}

}