#ifndef DEF_UNIT_MANAGER_HPP
#define DEF_UNIT_MANAGER_HPP

#include "base_unit.hpp"
#include "actor.hpp"

#include <vector>
#include <cstdint>

class unit_manager
{
    std::vector<std::vector<base_unit*>> units_pointer;
    std::vector<uint32_t> unit_id;

    void get(uint32_t id)
    {

    }

    //return handle
    void add(base_unit* unit)
    {
        unit_type t = unit->get_type();



    }


};
#endif
