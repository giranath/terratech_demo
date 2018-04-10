#include "unit_manager.hpp"
#include "../collision/collision_detector.hpp"

uint32_t unit_manager::get_unit_type(uint32_t id)
{
    return id & 0x0ff0000;
}

uint32_t unit_manager::actor_type_to_uint32_t(base_unit* unit)
{
    return static_cast<uint32_t>(unit->get_type()) << 16;
}

unit_manager::unit_manager(){}

base_unit* unit_manager::get(uint32_t id)
{
    uint32_t type = get_unit_type(id);
    if (type == 0 && units.contains(id))
    {
        std::lock_guard<std::mutex> lock(units_mutex);
        return &units[id];
    }
    else if (type == 1 && buildings.contains(id))
    {
        return &buildings[id];
    }

    return nullptr;
}

target_handle unit_manager::add(unit _unit, uint32_t id)
{
    std::lock_guard<std::mutex> lock(units_mutex);

    unit_id id_unit;
    id_unit.from_uint32_t(id);

    units.add(id, _unit);

    base_unit* u = &units[id];
    uint32_t type = id_unit.unit_type;

    u->set_id(id);
    return target_handle{ this, u };
}

target_handle unit_manager::add(building _unit, uint32_t id)
{
    unit_id id_unit;
    id_unit.from_uint32_t(id);
        
    buildings.add(id, _unit);

    base_unit* u = &buildings[id];
    uint32_t type = id_unit.unit_type;

    u->set_id(id);
    return target_handle{ this, u };
}

void unit_manager::remove(uint32_t id)
{
    unit_id id_unit;
    id_unit.from_uint32_t(id);

    if (id_unit.unit_type == 0)
    {
        std::lock_guard<std::mutex> lock(units_mutex);
        units.remove(id);
    }
    else if (id_unit.unit_type == 1)
    {
        buildings.remove(id);
    }
}

unit_manager::unit_iterator unit_manager::begin_of_units() {
    return units.begin();
}

unit_manager::unit_iterator unit_manager::end_of_units() {
    return units.end();
}

size_t unit_manager::count_units() const noexcept {
    std::lock_guard<std::mutex> lock(units_mutex);
    return units.size();
}

unit_manager::building_iterator unit_manager::begin_of_buildings() {
    return buildings.begin();
}

unit_manager::building_iterator unit_manager::end_of_buildings() {
    return buildings.end();
}

size_t unit_manager::count_buildings() const noexcept {
    return buildings.size();
}
