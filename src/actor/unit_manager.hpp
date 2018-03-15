#ifndef DEF_UNIT_MANAGER_HPP
#define DEF_UNIT_MANAGER_HPP

#include "base_unit.hpp"
#include "target_handle.hpp"

#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <memory>

class unit_manager
{
public:
    using unit_ptr = std::unique_ptr<base_unit>;
private:
    uint32_t unit_counter;

    std::vector<std::unordered_map<uint32_t, unit_ptr>> units;

public:
    unit_manager();

    uint32_t get_unit_type(uint32_t id);

    uint32_t actor_type_to_uint32_t(base_unit* unit);

    base_unit* get(uint32_t id);

    uint32_t get_next_id();

    target_handle add(unit_ptr unit);

    void remove(uint32_t id);

};
#endif
