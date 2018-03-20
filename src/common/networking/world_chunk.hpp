#ifndef DEF_WORLD_CHUNK_HPP
#define DEF_WORLD_CHUNK_HPP

#include <cstdint>
#include <vector>
#include <json/json.hpp>

namespace networking
{
    struct world_chunk
    {
        uint32_t x;
        uint32_t y;

        std::vector<uint8_t> regions_biome;

        world_chunk(uint32_t x, uint32_t y, const std::vector<uint8_t>& regions_biome) :
            x{ x },
            y{ y },
            regions_biome{ regions_biome } {
        }
    };

    void to_json(nlohmann::json& j, const world_chunk& c)
    {
        j = nlohmann::json{ { "x", c.x },{ "y", c.y },{ "biomes", c.regions_biome } };
    }

    void from_json(const nlohmann::json& j, world_chunk& c)
    {
        c.x = j.at("x");
        c.y = j.at("y");
        c.regions_biome = j.at("biomes").get< std::vector<uint8_t>>();
    }
}
#endif
