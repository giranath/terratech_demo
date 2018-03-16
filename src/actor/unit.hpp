#ifndef DEF_UNIT_HPP
#define DEF_UNIT_HPP

#include "base_unit.hpp"
#include "ressource_value.hpp"
#include "target_handle.hpp"
#include "unit_flyweight.hpp"
#include "actor.hpp"

#include <vector>
#include <cstdint>
class unit : public base_unit
{
    ressource_value transported_ressource;
    target_handle target;
    glm::vec2 target_position;
public:

    unit(glm::vec3 position, glm::vec2 target_position, unit_flyweight* unit_fly, unit_manager* manager) :
        base_unit(position, unit_fly, actor_type::unit),
        transported_ressource{},
        target{manager},
        target_position{target_position}
    {}

    void set_target(target_handle _target)
    {
        target = _target;
    }

    target_handle& get_target()
    {
        return target;
    }

    void set_target_position(glm::vec2 _target_position)
    {
        target_position = _target_position;
    }

    glm::vec2& get_target_position()
    {
        return target_position;
    }

    void embark_in_target()
    {
        base_unit* targ = target.get();
        if (targ)
        {
            //targ->load_unit(this);
        }
    }

    //todo do
    void gather_ressource()
    {
    }
};
#endif
