#ifndef DEF_WORLD_CHUNK_HPP
#define DEF_WORLD_CHUNK_HPP

#include <cstdint>
#include <vector>
#include <json/json.hpp>

namespace networking
{
    struct resource {
        uint32_t x = 0;
        uint32_t y = 0;
        uint8_t type = 0;
        int quantity = 0;

        resource() = default;

        resource(uint32_t x, uint32_t y, uint8_t type, int quantity)
        : x{x}
        , y{y}
        , type{type}
        , quantity{quantity} {

        }
    };

    void to_json(nlohmann::json& j, const resource& c)
    {
        j = nlohmann::json {
            { "x", c.x },
            { "y", c.y },
            { "type", c.type },
            { "quantity", c.quantity }
        };
    }

    void from_json(const nlohmann::json& j, resource& c)
    {
        c.x = j.at("x");
        c.y = j.at("y");
        c.type = j.at("type");
        c.quantity = j.at("quantity");
    }

    struct world_chunk
    {
        uint32_t x;
        uint32_t y;

        std::vector<uint8_t> regions_biome;
        std::vector<resource> sites;

        world_chunk(uint32_t x = 0, uint32_t y = 0, const std::vector<uint8_t>& regions_biome = {}, const std::vector<resource>& sites = {}) :
            x{ x },
            y{ y },
            regions_biome{ regions_biome },
            sites{ sites } {
        }
    };

    void to_json(nlohmann::json& j, const world_chunk& c)
    {
        j = nlohmann::json {
            { "x", c.x },
            { "y", c.y },
            { "biomes", c.regions_biome },
            { "sites", c.sites }
        };
    }

    void from_json(const nlohmann::json& j, world_chunk& c)
    {
        c.x = j.at("x");
        c.y = j.at("y");
        c.regions_biome = j.at("biomes").get< std::vector<uint8_t>>();
        c.sites = j.at("sites").get<std::vector<resource>>();
    }
}
#endif
