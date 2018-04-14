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
    unit();
    unit(glm::vec3 position, glm::vec2 target_position, unit_flyweight* unit_fly, unit_manager* manager);
    unit(glm::vec3 position, unit_flyweight* unit_fly, unit_manager* manager);
    void set_target(target_handle _target);
    target_handle& get_target();
    void set_target_position(glm::vec2 _target_position);
    glm::vec2& get_target_position();
    const glm::vec2& get_target_position() const;
    void embark_in_target();
    void gather_ressource();

    friend void to_json(nlohmann::json& j, const unit& u);
    friend void from_json(const nlohmann::json& j, unit& u);
};

#endif
