#ifndef DEF_UNIT_HPP
#define DEF_UNIT_HPP

#include "base_unit.hpp"
#include "ressource_value.hpp"
#include <vector>

class unit : public base_unit
{
    ressource_value transported_ressource;
    
    //TODO change to target Handle
    base_unit* target;

    void embark_in_target()
    {
        target->load_unit(this);
    }

    void gather_ressource()
    {
    }
};
#endif
