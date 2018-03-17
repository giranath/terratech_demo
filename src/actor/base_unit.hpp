#ifndef DEF_BASE_UNIT_HPP
#define DEF_BASE_UNIT_HPP

#include "actor.hpp"
#include "unit_flyweight.hpp"

class base_unit : public actor
{
    unit_flyweight* flyweight;
    int current_health;
    uint32_t id;

public:
    base_unit(glm::vec3 position, unit_flyweight* definition, actor_type type)
    : actor(position, true, true, type)
    , flyweight(definition)
    , current_health(flyweight->get_max_health()) {

    }

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

    void set_id(uint32_t new_id) noexcept {
        id = new_id;
    }

    uint32_t get_id() const noexcept {
        return id;
    }

	float get_speed()
	{
		return flyweight->get_speed();
	}

};
#endif
