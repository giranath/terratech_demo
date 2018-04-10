#include "update_player_visibility.hpp"

#include <algorithm>
#include <iterator>

namespace task {

update_player_visibility::update_player_visibility(uint8_t player, const visibility_map& v, unit_manager& units)
: player_id(player)
, visibility_(v)
, units_(units) {

}

void update_player_visibility::execute() {
    visibility_.clear();

    /*auto units = units_.units_of(player_id);
    std::for_each(std::begin(units), std::end(units), [this](unit* u) {
        const int start_of_x = std::floor(u->get_position().x - u->visibility_radius());
        const int start_of_y = std::floor(u->get_position().z - u->visibility_radius());
        const int end_of_x = std::ceil(u->get_position().x + u->visibility_radius());
        const int end_of_y = std::ceil(u->get_position().z + u->visibility_radius());

        for(int y = std::max(0, start_of_y); y < std::min(static_cast<int>(visibility_.height()), end_of_y); ++y) {
            for(int x = std::max(0, start_of_x); x < std::min(static_cast<int>(visibility_.width()), end_of_x); ++x) {
                // Test each tiles
                const glm::vec3 tile_pos(x, 0, y);
                const glm::vec3 diff = tile_pos - u->get_position();
                const float len = glm::length(diff);

                if(len <= u->visibility_radius()) {
                    visibility_.set(x, y, visibility::visible);
                }
            }
        }
    });*/
}

uint8_t update_player_visibility::get_player() const noexcept {
    return player_id;
}

const visibility_map& update_player_visibility::visibility() const noexcept {
    return visibility_;
}

}