#ifndef MMAP_DEMO_SERVER_UNIT_MANAGER_HPP
#define MMAP_DEMO_SERVER_UNIT_MANAGER_HPP

#include "../common/actor/unit_manager.hpp"

#include <unordered_map>

class server_unit_manager : public unit_manager {
    std::unordered_map<uint8_t, uint16_t> player_counters;
public:
    server_unit_manager(memory::heap_allocator& allocator);
    target_handle add_unit_to(uint8_t player_id, unit_ptr unit);
    void add_player(uint8_t player_id);
};


#endif //MMAP_DEMO_SERVER_UNIT_MANAGER_HPP
