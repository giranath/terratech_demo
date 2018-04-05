#include "update_target.hpp"

#include "../actor/actor.hpp"

namespace networking {

void to_json(nlohmann::json& j, const update_target& c) {
    j = nlohmann::json {
            { "target", c.new_target },
            { "unit", c.unit_id }
    };
}

void from_json(const nlohmann::json& j, update_target& c) {
    c.new_target = j.at("target").get<glm::vec2>();
    c.unit_id = j.at("unit");
}

}
