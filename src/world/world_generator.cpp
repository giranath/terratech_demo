#include "world_generator.hpp"

#include <vector>

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

world_generator::world_generator(uint32_t seed, uint32_t chunk_width, uint32_t chunk_depth)
: map_generator{terra_map_create(seed, chunk_width, 1, chunk_depth, CHUNK_NOISE_RATIO)}
, altitude_layer{terra_map_add_noise_layer(map_generator, TERRA_NOISE_OCTAVE_PERLIN)}
, humidity_layer{terra_map_add_noise_layer(map_generator, TERRA_NOISE_OCTAVE_PERLIN)}
, temperature_layer{terra_map_add_noise_layer(map_generator, TERRA_NOISE_OCTAVE_PERLIN)}{
    if(altitude_layer == TERRA_LAYER_INVALID
    || humidity_layer == TERRA_LAYER_INVALID
    || temperature_layer == TERRA_LAYER_INVALID) {
        throw invalid_layer_exception{};
    }

    setup_biomes();
    setup_resources();
}

world_generator::~world_generator() {
    terra_map_destroy(map_generator);
}

void world_generator::setup_biomes() {
    // The biome table
    terra_biome_table* table = terra_biome_table_create(6, 4);

    terra_biome_table_set(table, 0, 0, BIOME_TUNDRA);
    terra_biome_table_set(table, 0, 0, BIOME_TUNDRA);
    terra_biome_table_set(table, 1, 0, BIOME_TUNDRA);
    terra_biome_table_set(table, 2, 0, BIOME_GRASS_DESERT);
    terra_biome_table_set(table, 3, 0, BIOME_GRASS_DESERT);
    terra_biome_table_set(table, 4, 0, BIOME_DESERT);
    terra_biome_table_set(table, 5, 0, BIOME_DESERT);

    terra_biome_table_set(table, 0, 1, BIOME_TUNDRA);
    terra_biome_table_set(table, 1, 1, BIOME_TAIGA);
    terra_biome_table_set(table, 2, 1, BIOME_WOODS);
    terra_biome_table_set(table, 3, 1, BIOME_WOODS);
    terra_biome_table_set(table, 4, 1, BIOME_SAVANNA);
    terra_biome_table_set(table, 5, 1, BIOME_SAVANNA);

    terra_biome_table_set(table, 0, 2, BIOME_TAIGA);
    terra_biome_table_set(table, 1, 2, BIOME_TAIGA);
    terra_biome_table_set(table, 2, 2, BIOME_FOREST);
    terra_biome_table_set(table, 3, 2, BIOME_FOREST);
    terra_biome_table_set(table, 4, 2, BIOME_SEASONAL_FOREST);
    terra_biome_table_set(table, 5, 2, BIOME_SEASONAL_FOREST);

    terra_biome_table_set(table, 0, 3, BIOME_FOREST);
    terra_biome_table_set(table, 1, 3, BIOME_FOREST);
    terra_biome_table_set(table, 2, 3, BIOME_FOREST);
    terra_biome_table_set(table, 3, 3, BIOME_SWAMP);
    terra_biome_table_set(table, 4, 3, BIOME_RAIN_FOREST);
    terra_biome_table_set(table, 5, 3, BIOME_RAIN_FOREST);

    // The altitude axis
    terra_biome_table* altitude_axis = terra_biome_table_create(20, 0);

    terra_biome_table_set(altitude_axis, 0,  0, BIOME_DEEP_WATER);
    terra_biome_table_set(altitude_axis, 1,  0, BIOME_DEEP_WATER);
    terra_biome_table_set(altitude_axis, 2,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 3,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 4,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 5,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 6,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 12, 0, BIOME_WATER);

    // Here we create the biome generator
    terra_biome_generator* generator = terra_biome_generator_create(TERRA_BIOME_GENERATOR_TABLE);

    terra_biome_generator_set_table(generator, TERRA_BIOMEGEN_BIOME_TABLE,  table);
    terra_biome_generator_set_layer(generator, TERRA_BIOMEGEN_AXIS_X_NOISE, terra_map_get_layer(map_generator, temperature_layer));
    terra_biome_generator_set_layer(generator, TERRA_BIOMEGEN_AXIS_Y_NOISE, terra_map_get_layer(map_generator, humidity_layer));

    terra_biome_generator_set_table(generator, TERRA_BIOMEGEN_BIOME_AXIS,   altitude_axis);
    terra_biome_generator_set_layer(generator, TERRA_BIOMEGEN_AXIS_Z_NOISE, terra_map_get_layer(map_generator, altitude_layer));

    // Here we attach the biome generator to the map
    terra_map_set_biome_generator(map_generator, generator);

    // Releases the tables
    terra_biome_generator_destroy(generator);
    terra_biome_table_destroy(altitude_axis);
    terra_biome_table_destroy(table);
}

void world_generator::setup_resources() {
    static const std::vector<int> SITES_DISTRIBUTION_BIOMES = {
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
            BIOME_DEEP_WATER
    };

    static const std::vector<std::vector<int>> SITES_DISTRIBUTION_SITES = {
            { SITE_NOTHING, SITE_DEER,    SITE_COAL,   SITE_OIL   },
            { SITE_NOTHING, SITE_HORSES,  SITE_STONE              },
            { SITE_NOTHING, SITE_URANIUM, SITE_IRON,   SITE_OIL   },
            { SITE_NOTHING, SITE_DEER,    SITE_CATTLE, SITE_IRON  },
            { SITE_NOTHING, SITE_DEER,    SITE_CATTLE, SITE_IRON  },
            { SITE_NOTHING, SITE_CATTLE,  SITE_WHEAT,  SITE_STONE },
            { SITE_NOTHING, SITE_DEER,    SITE_BANANA, SITE_SHEEP },
            { SITE_NOTHING, SITE_DEER,    SITE_BANANA, SITE_SHEEP },
            { SITE_NOTHING, SITE_BANANA,  SITE_FISH,   SITE_COAL  },
            { SITE_NOTHING, SITE_DEER,    SITE_SHEEP,  SITE_WHEAT },
            { SITE_NOTHING, SITE_FISH                             },
            { SITE_NOTHING, SITE_FISH,    SITE_PEARLS             }

    };

    static const std::vector<std::vector<double>> SITES_DISTRIBUTION_WEIGHTS = {
            { 200.0, 2,   1,   1   },
            { 200.0, 1,   1        },
            { 200.0, 0.5, 1,   3   },
            { 200.0, 1,   1,   1   },
            { 200.0, 1,   1,   1   },
            { 200.0, 1,   1.5, 0.5 },
            { 200.0, 1,   1.5, 0.5 },
            { 200.0, 1,   1.5, 0.5 },
            { 200.0, 1,   0.5, 0.5 },
            { 200.0, 2,   1,   1   },
            { 200.0, 1             },
            { 200.0, 1,   1        }
    };

    terra_site_distrib_table* table = terra_site_distrib_table_create();

    for(std::size_t i = 0; i < SITES_DISTRIBUTION_BIOMES.size(); ++i) {
        terra_site_distrib_table_set(table, SITES_DISTRIBUTION_BIOMES[i],
                                     SITES_DISTRIBUTION_SITES[i].size(),
                                     &SITES_DISTRIBUTION_SITES[i][0],
                                     &SITES_DISTRIBUTION_WEIGHTS[i][0]);
    }

    terra_site_generator* site_generator = terra_site_generator_create(TERRA_SITE_GENERATOR_WEIGHT);
    terra_site_generator_set_distrib_table(site_generator, TERRA_SITEGEN_DISTRIBUTION, table);

    terra_map_set_site_generator(map_generator, site_generator);

    terra_site_generator_destroy(site_generator);
    terra_site_distrib_table_destroy(table);
}