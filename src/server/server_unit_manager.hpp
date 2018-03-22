#ifndef MMAP_DEMO_SERVER_UNIT_MANAGER_HPP
#define MMAP_DEMO_SERVER_UNIT_MANAGER_HPP

#include "../common/actor/unit_manager.hpp"

class server_unit_manager : public unit_manager {
    std::vector<uint16_t> player_counters;
public:
    target_handle add_unit_to(uint8_t player_id, unit_ptr unit);
};


#endif //MMAP_DEMO_SERVER_UNIT_MANAGER_HPP
