#include "unit_manager.hpp"

    uint32_t unit_manager::get_unit_type(uint32_t id)
    {
        return id >> 24;
    }

    uint32_t unit_manager::actor_type_to_uint32_t(base_unit* unit)
    {
        return static_cast<uint32_t>(unit->get_type()) << 24;
    }

    unit_manager::unit_manager() :
        unit_counter{},
        units{ static_cast<uint32_t>(actor_type::MAX_ACTOR_TYPE) }
    {}

    base_unit* unit_manager::get(uint32_t id)
    {
        uint32_t type = get_unit_type(id);

        auto it = units[type].find(id);
        if (it != units[type].end())
        {
            return it->second;
        }

        return nullptr;
    }

    uint32_t unit_manager::get_next_id()
    {
        return unit_counter;
    }

    target_handle unit_manager::add(base_unit* unit)
    {
        uint32_t type = actor_type_to_uint32_t(unit);
        units[type >> 24][type + unit_counter] = unit;
        unit->set_id(type + unit_counter);
        ++unit_counter;
        return target_handle{this, unit};
    }

    void unit_manager::remove(uint32_t id)
    {
        uint32_t type = get_unit_type(id);
        units[type].erase(id);
    }
