#ifndef MMAP_DEMO_WORLD_GENERATOR_H
#define MMAP_DEMO_WORLD_GENERATOR_H

#include <terratech/terratech.h>
#include <cstdint>

class invalid_layer_exception{};

class world_generator;
class world_generator_chunk {
    friend world_generator;
    terra_chunk* chunk;

    world_generator_chunk(terra_chunk* chunk);
public:
    world_generator_chunk(const world_generator_chunk&) = delete;
    world_generator_chunk& operator=(const world_generator_chunk&) = delete;

    world_generator_chunk(world_generator_chunk&&) = default;
    world_generator_chunk& operator=(world_generator_chunk&&) = default;

    ~world_generator_chunk();

    operator terra_chunk*() noexcept;
    operator const terra_chunk*() noexcept;
};

class world_generator {
    terra_map* map_generator;
    terra_layer_handle altitude_layer;
    terra_layer_handle humidity_layer;
    terra_layer_handle temperature_layer;

    static constexpr float CHUNK_NOISE_RATIO = 0.25f;

    void setup_biomes();
    void setup_resources();

public:
    enum {
        BIOME_RAIN_FOREST,
        BIOME_SWAMP,
        BIOME_SEASONAL_FOREST,
        BIOME_FOREST,
        BIOME_TAIGA,
        BIOME_WOODS,
        BIOME_SAVANNA,
        BIOME_DESERT,
        BIOME_GRASS_DESERT,
        BIOME_TUNDRA,
        BIOME_WATER,
        BIOME_DEEP_WATER,
        BIOME_COUNT
    };

    enum {
        SITE_NOTHING,
        SITE_HORSES,
        SITE_IRON,
        SITE_COAL,
        SITE_OIL,
        SITE_ALUMINUM,
        SITE_URANIUM,
        SITE_BANANA,
        SITE_CATTLE,
        SITE_DEER,
        SITE_SHEEP,
        SITE_WHEAT,
        SITE_STONE,
        SITE_PEARLS,
        SITE_FISH,
    };

    world_generator(uint32_t seed, uint32_t chunk_width, uint32_t chunk_depth);
    ~world_generator();

    world_generator_chunk generate_chunk(int x, int y, int z) const noexcept;
};


#endif
