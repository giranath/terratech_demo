#ifndef DEF_BASE_UNIT_HPP
#define DEF_BASE_UNIT_HPP

#include "unit_fly_weight.hpp"
#include "actor.hpp"
#include "target_handle.hpp"

#include <cstdint>

class unit;

class base_unit : public actor
{
    uint32_t id;

    unit_fly_weight* unit_fly;

    uint8_t Transported_unit_amount;
    //check type
    std::vector<target_handle> transported_units;

    
    int current_health;

public:

    base_unit(glm::vec3 position, actor_type type, unit_fly_weight* unit_fly, unit_manager* manager) : 
        actor{position, true, true, type},
        unit_fly{unit_fly},
        Transported_unit_amount{ unit_fly->get_tranport_unit_capacity()},
        transported_units{ unit_fly->get_tranport_unit_capacity(), target_handle{ manager } },
        current_health{unit_fly ->get_max_health()}
    {}

    void set_id(uint32_t _id)
    {
        id = _id;
    }

    uint32_t get_id()
    {
        return id;
    }

    void load_unit(unit* u)
    {
        /*if (transported_units.size() < Transported_unit_amount)
        {
            transported_units.push_back(u);
        }*/
    }

    void take_damage(const unsigned int& damage)
    {
        current_health -= damage;
    }

    void heal(const unsigned int& heal)
    {
        current_health += heal;
    }

    bool is_dead()
    {
        return current_health <= 0;
    }
};
#endif
