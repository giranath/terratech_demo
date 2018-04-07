#ifndef MMAP_DEMO_PLAYER_INIT_HPP
#define MMAP_DEMO_PLAYER_INIT_HPP

#include <cstdint>
#include <json/json.hpp>

namespace networking {

struct player_infos {
    uint8_t id {};

    player_infos() = default;
    player_infos(uint8_t id);
};

void to_json(nlohmann::json& j, const player_infos& infos);
void from_json(const nlohmann::json& j, player_infos& infos);

}

#endif //MMAP_DEMO_PLAYER_INIT_HPP
