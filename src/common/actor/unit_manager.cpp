#include "unit_manager.hpp"


unit_manager::unit_manager(memory::heap_allocator& allocator)
: units{memory::container_heap_allocator<unit_collection>(allocator)} {
    for(uint32_t i = 0; i < static_cast<uint32_t>(actor_type::MAX_ACTOR_TYPE); ++i) {
        units.emplace_back(memory::container_heap_allocator<std::pair<const uint32_t, unit_ptr>>(allocator));
    }
}

uint32_t unit_manager::get_unit_type(uint32_t id)
{
    return id & 0x0ff0000;
}

uint32_t unit_manager::actor_type_to_uint32_t(base_unit* unit)
{
    return static_cast<uint32_t>(unit->get_type()) << 16;
}


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

target_handle unit_manager::add(unit_ptr unit, uint32_t id)
{
    if (unit)
    {
        unit_id id_unit;
        id_unit.from_uint32_t(id);
        uint32_t type = id_unit.unit_type;

        base_unit* temp_ptr = unit.get();
        units[type][id] = std::move(unit);
        temp_ptr->set_id(id);
        return target_handle{ this, temp_ptr };
    }

    return {};
}

void unit_manager::remove(uint32_t id)
{
    unit_id id_unit;
    id_unit.from_uint32_t(id);
    uint32_t type = id_unit.unit_type;
    units[type].erase(id);
}

unit_manager::iterator unit_manager::begin_of_units() {
    return units[static_cast<std::size_t>(actor_type::unit)].begin();
}

unit_manager::iterator unit_manager::end_of_units() {
    return units[static_cast<std::size_t>(actor_type::unit)].end();
}
