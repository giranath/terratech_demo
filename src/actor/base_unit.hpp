#ifndef DEF_BASE_UNIT_HPP
#define DEF_BASE_UNIT_HPP

#include "unit_fly_weight.hpp"
class base_unit
{
    unit_fly_weight* unit_fly_weight;
    int current_health;

    void take_damage(unsigned int damage)
    {
        current_health -= damage;
    }

    void heal(unsigned int heal)
    {
        current_health += heal;
    }

    bool is_dead()
    {
        return current_health <= 0;
    }
};
#endif
