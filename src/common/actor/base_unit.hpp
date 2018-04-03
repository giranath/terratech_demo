#ifndef DEF_BASE_UNIT_HPP
#define DEF_BASE_UNIT_HPP

#include "actor.hpp"
#include "unit_flyweight.hpp"

#include <json/json.hpp>

class base_unit : public actor
{
    unit_flyweight* flyweight;
    int flyweight_id;

    int current_health;
    uint32_t id;

public:
    base_unit(glm::vec3 position = {}, unit_flyweight* definition = nullptr, actor_type type = actor_type::MAX_ACTOR_TYPE)
    : actor(position, true, true, type)
    , flyweight(definition)
    , flyweight_id(flyweight ? flyweight->id() : unit_flyweight::INVALID_ID)
    , current_health(flyweight ? flyweight->get_max_health() : 0) {

    }

    void set_flyweight(unit_flyweight* new_flyweight) {
        flyweight = new_flyweight;
        current_health = flyweight->get_max_health();
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
        return flyweight_id;
    }

	float get_speed()
	{
		return flyweight->get_speed();
	}

    const std::string& texture() const noexcept {
        return flyweight->texture();
    }

    float visibility_radius() const noexcept {
        return flyweight->visibility();
    }

    friend void from_json(const nlohmann::json& j, base_unit& u);
    friend void to_json(nlohmann::json& j, const base_unit& u);
};


#endif
