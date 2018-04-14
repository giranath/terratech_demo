#ifndef MMAP_DEMO_BUILD_HPP
#define MMAP_DEMO_BUILD_HPP

#include <cstdint>
#include <glm/glm.hpp>
#include <json/json.hpp>

namespace networking {

struct build_building {
    uint8_t building_id;
    uint32_t builder_id;
    glm::vec2 position;

    build_building(uint8_t building, uint32_t builder, glm::vec2 position);
};

void to_json(nlohmann::json& j, const build_building& c);
void from_json(const nlohmann::json& j, build_building& c);

}

#endif //MMAP_DEMO_BUILD_HPP
