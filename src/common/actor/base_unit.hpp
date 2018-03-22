#ifndef DEF_BASE_UNIT_HPP
#define DEF_BASE_UNIT_HPP

#include "actor.hpp"
#include "unit_flyweight.hpp"

#include <json/json.hpp>

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

    int get_type_id() const noexcept {
        return flyweight->id();
    }

	float get_speed()
	{
		return flyweight->get_speed();
	}

    const std::string& texture() const noexcept {
        return flyweight->texture();
    }

    friend void from_json(const nlohmann::json& j, base_unit& u);
    friend void to_json(nlohmann::json& j, const base_unit& u);
};


#endif
