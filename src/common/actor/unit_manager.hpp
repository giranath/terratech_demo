#ifndef DEF_UNIT_MANAGER_HPP
#define DEF_UNIT_MANAGER_HPP

#include "base_unit.hpp"
#include "target_handle.hpp"

#include <vector>
#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <memory>

struct unit_id
{
    uint8_t player_id;
    uint8_t unit_type;
    uint16_t counter;
    uint32_t to_uint32_t() const
    {
        return *reinterpret_cast<const uint32_t*>(this);
    }

    void from_uint32_t(uint32_t val)
    {
        *this = *reinterpret_cast<unit_id*>(&val);
    }
};

class unit_manager
{
public:
    using unit_ptr = std::unique_ptr<base_unit>;
    using iterator = std::unordered_map<uint32_t, unit_ptr>::iterator;
private:
    std::vector<std::unordered_map<uint32_t, unit_ptr>> units;

public:
    unit_manager();

    uint32_t get_unit_type(uint32_t id);

    uint32_t actor_type_to_uint32_t(base_unit* unit);

    base_unit* get(uint32_t id);

    target_handle add(unit_ptr unit, uint32_t id);

    void remove(uint32_t id);

    iterator begin_of_units();
    iterator end_of_units();

};
#endif
