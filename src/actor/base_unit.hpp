#ifndef DEF_BASE_UNIT_HPP
#define DEF_BASE_UNIT_HPP

#include "unit_fly_weight.hpp"
#include "actor.hpp"
class unit;

class base_unit : public actor
{
    unit_fly_weight* unit_fly_weight;

    int Transported_unit_amount;
    std::vector<unit*> transported_units;

    int current_health;
public:
    void load_unit(unit* u)
    {
        if (transported_units.size() < Transported_unit_amount)
        {
            transported_units.push_back(u);
        }
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
