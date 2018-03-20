#ifndef DEF_WORLD_MAP_HPP
#define DEF_WORLD_MAP_HPP

#include <cstdint>
#include <json/json.hpp>

namespace networking
{
    struct world_map
    {
        uint16_t chunk_width;
        uint16_t chunk_height;

        world_map(uint16_t chunk_width, uint16_t chunk_height):
            chunk_width{chunk_width},
            chunk_height{chunk_height}{
        }
    };

    void to_json(nlohmann::json& j, const world_map& m)
    {
        j = nlohmann::json{ { "chunk_width", m.chunk_width },{ "chunk_height", m.chunk_height }};
    }

    void from_json(const nlohmann::json& j, world_map& m)
    {
        m.chunk_width = j.at("chunk_width");
        m.chunk_height = j.at("chunk_height");
    }
}

#endif