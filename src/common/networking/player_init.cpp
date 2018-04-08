#include "player_init.hpp"

namespace networking {

player_infos::player_infos(uint8_t id)
: id{id} {

}

void to_json(nlohmann::json& j, const player_infos& infos) {
    j["id"] = infos.id;
}

void from_json(const nlohmann::json& j, player_infos& infos) {
    infos.id = j["id"];
}

}