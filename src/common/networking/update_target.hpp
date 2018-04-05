#ifndef MMAP_DEMO_UPDATE_TARGET_HPP
#define MMAP_DEMO_UPDATE_TARGET_HPP

#include <glm/glm.hpp>
#include <json/json.hpp>

namespace networking {

struct update_target {
    glm::vec2 new_target;
    int unit_id = -1;

    update_target() = default;
    update_target(int id, glm::vec2 pos)
    : new_target(pos)
    , unit_id(id) {

    }
};

void to_json(nlohmann::json& j, const update_target& c);
void from_json(const nlohmann::json& j, update_target& c);

}

#endif //MMAP_DEMO_UPDATE_TARGET_HPP
