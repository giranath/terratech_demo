#include "unit.hpp"

unit::unit(glm::vec3 position, glm::vec2 target_position, unit_flyweight* unit_fly, unit_manager* manager) :
base_unit(position, unit_fly, actor_type::unit),
transported_ressource{},
target{manager},
target_position{target_position}
{}

void unit::set_target(target_handle _target)
{
    target = _target;
}

target_handle& unit::get_target()
{
    return target;
}

void unit::set_target_position(glm::vec2 _target_position)
{
    target_position = _target_position;
}

glm::vec2& unit::get_target_position()
{
    return target_position;
}

const glm::vec2& unit::get_target_position() const
{
    return target_position;
}

void unit::embark_in_target()
{
    base_unit* targ = target.get();
    if (targ)
    {
        //targ->load_unit(this);
    }
}

//todo do
void unit::gather_ressource()
{
}

void to_json(nlohmann::json& j, const unit& u) {
    to_json(j, static_cast<const base_unit&>(u));
    j["target_position"] = u.target_position;
}

void from_json(const nlohmann::json& j, unit& u) {
    from_json(j, static_cast<base_unit&>(u));
    u.target_position = j["target_position"].get<glm::vec2>();
}