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
            return it->second.get();
        }

        return nullptr;
    }

    uint32_t unit_manager::get_next_id()
    {
        return unit_counter;
    }

    target_handle unit_manager::add(unit_ptr unit)
    {
        uint32_t type = actor_type_to_uint32_t(unit.get());
        base_unit* temp_ptr = unit.get();
        units[type >> 24][type + unit_counter] = std::move(unit);
        temp_ptr->set_id(type + unit_counter);
        ++unit_counter;
        return target_handle{this, temp_ptr};
    }

    void unit_manager::remove(uint32_t id)
    {
        uint32_t type = get_unit_type(id);
        units[type].erase(id);
    }

unit_manager::iterator unit_manager::begin_of_units() {
    return units[static_cast<std::size_t>(actor_type::unit)].begin();
}

unit_manager::iterator unit_manager::end_of_units() {
    return units[static_cast<std::size_t>(actor_type::unit)].end();
}
