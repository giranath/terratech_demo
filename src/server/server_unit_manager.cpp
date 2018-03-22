#include "server_unit_manager.hpp"

target_handle server_unit_manager::add_unit_to(uint8_t player_id, unit_ptr unit) {
    if(!unit) { return {}; }

    unit_id real_id;
    real_id.player_id = player_id;
    real_id.unit_type = static_cast<uint8_t>(unit->get_type());
    real_id.counter = player_counters[player_id]++;

    return add(std::move(unit), real_id.to_uint32_t());
}