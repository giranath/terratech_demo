#ifndef DEF_UNIT_HPP
#define DEF_UNIT_HPP

#include "base_unit.hpp"
#include "ressource_value.hpp"
#include "target_handle.hpp"
#include "unit_fly_weight.hpp"
#include "actor.hpp"

#include <vector>
#include <cstdint>
class unit : public base_unit
{
    ressource_value transported_ressource;
    target_handle target;
public:

    unit(glm::vec3 position, unit_fly_weight* unit_fly, unit_manager* manager) :
        base_unit{position, actor_type::unit, unit_fly, manager},
        transported_ressource{},
        target{manager}
    {}

    void embark_in_target()
    {
        base_unit* targ = target.get();
        if (targ)
        {
            targ->load_unit(this);
        }
    }

    //todo do
    void gather_ressource()
    {
    }
};
#endif
