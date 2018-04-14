#include "build.hpp"

#include "../actor/actor.hpp"

namespace networking {

build_building::build_building(uint8_t building, uint32_t builder, glm::vec2 position)
: building_id{building}
, builder_id{builder}
, position{position} {

}

void to_json(nlohmann::json& j, const build_building& c) {
    j["builder"] = c.builder_id;
    j["building"] = c.building_id;
    j["position"] = c.position;
}

void from_json(const nlohmann::json& j, build_building& c) {
    c.builder_id = j["builder"];
    c.building_id = j["building"];
    c.position = j["position"].get<glm::vec2>();
}

}