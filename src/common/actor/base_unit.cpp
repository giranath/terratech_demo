#include "base_unit.hpp"

void to_json(nlohmann::json& j, const base_unit& u) {
    to_json(j, static_cast<const actor&>(u));
    j["current_health"] = u.current_health;
    j["id"] = u.id;
    j["flyweight"] = u.flyweight->id();
}

void from_json(const nlohmann::json& j, base_unit& u) {
    from_json(j, static_cast<actor&>(u));
    u.current_health = j["current_health"];
    u.id = j["id"];
    u.flyweight_id = j["flyweight"];
}