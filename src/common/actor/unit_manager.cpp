#include "unit_manager.hpp"

uint32_t unit_manager::get_unit_type(uint32_t id)
{
    return id & 0x0ff0000;
}

uint32_t unit_manager::actor_type_to_uint32_t(base_unit* unit)
{
    return static_cast<uint32_t>(unit->get_type()) << 16;
}

unit_manager::unit_manager() :
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

std::size_t unit_manager::count_units() const noexcept {
    return units[static_cast<std::size_t>(actor_type::unit)].size();
}

std::vector<unit*> unit_manager::units_of(uint8_t player_id) {
    std::vector<base_unit*> units_pts;
    units_pts.reserve(count_units());

    std::transform(begin_of_units(), end_of_units(), std::back_inserter(units_pts), [](auto& p) {
        return p.second.get();
    });

    std::vector<unit*> units;
    units.reserve(units_pts.size());
    std::transform(std::begin(units_pts), std::end(units_pts), std::back_inserter(units), [](base_unit* u) {
        return static_cast<unit*>(u);
    });

    auto end = std::copy_if(std::begin(units), std::end(units), std::begin(units), [player_id](unit* u) {
        return unit_id(u->get_id()).player_id == player_id;
    });

    units.resize(std::distance(std::begin(units), end));

    return units;
}