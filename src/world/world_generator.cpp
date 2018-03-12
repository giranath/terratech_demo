#include "world_generator.hpp"

#include <vector>
#include <iostream>

world_generator_chunk::world_generator_chunk(terra_chunk* chunk)
: chunk {chunk} {

}

world_generator_chunk::~world_generator_chunk() {
    terra_chunk_release(chunk);
}

world_generator_chunk::operator terra_chunk*() noexcept {
    return chunk;
}

world_generator_chunk::operator const terra_chunk*() noexcept {
    return chunk;
}

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

world_generator_chunk world_generator::generate_chunk(int x, int y, int z) const noexcept {
    terra_chunk* chunk = terra_map_generate_chunk(map_generator, x, z);

    return world_generator_chunk{chunk};
}

terra_biome_table* make_biome_table() {
    terra_biome_table* table = terra_biome_table_create(5, 4);

    terra_biome_table_set(table, 0, 0, BIOME_SNOW);
    terra_biome_table_set(table, 1, 0, BIOME_ROCK);
    terra_biome_table_set(table, 2, 0, BIOME_ROCK);
    terra_biome_table_set(table, 3, 0, BIOME_DESERT);
    terra_biome_table_set(table, 4, 0, BIOME_DESERT);

    terra_biome_table_set(table, 0, 1, BIOME_SNOW);
    terra_biome_table_set(table, 1, 1, BIOME_ROCK);
    terra_biome_table_set(table, 2, 1, BIOME_GRASS);
    terra_biome_table_set(table, 3, 1, BIOME_ROCK);
    terra_biome_table_set(table, 4, 1, BIOME_DESERT);

    terra_biome_table_set(table, 0, 2, BIOME_GRASS);
    terra_biome_table_set(table, 1, 2, BIOME_GRASS);
    terra_biome_table_set(table, 2, 2, BIOME_GRASS);
    terra_biome_table_set(table, 3, 2, BIOME_ROCK);
    terra_biome_table_set(table, 4, 2, BIOME_DESERT);

    terra_biome_table_set(table, 0, 3, BIOME_GRASS);
    terra_biome_table_set(table, 1, 3, BIOME_GRASS);
    terra_biome_table_set(table, 2, 3, BIOME_GRASS);
    terra_biome_table_set(table, 3, 3, BIOME_GRASS);
    terra_biome_table_set(table, 4, 3, BIOME_GRASS);

    return table;
}

terra_biome_table* make_altitude_axis() {
    terra_biome_table* altitude_axis = terra_biome_table_create(10, 0);

    terra_biome_table_set(altitude_axis, 0,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 1,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 2,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 9, 0, BIOME_WATER);

    return altitude_axis;
}

void world_generator::setup_biomes() {
    // The biome table
    terra_biome_table* table = make_biome_table();

    // The altitude axis
    terra_biome_table* altitude_axis = make_altitude_axis();

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

terra_site_distrib_table* make_site_distribution() {
    static const std::vector<int> SITES_DISTRIBUTION_BIOMES = {
            BIOME_GRASS,
            BIOME_SNOW,
            BIOME_ROCK,
            BIOME_DESERT,
            BIOME_WATER,
    };

    static const std::vector<std::vector<int>> SITES_DISTRIBUTION_SITES = {
            { SITE_NOTHING, SITE_DEER,    SITE_GOLD,   SITE_STONE, SITE_TREE, SITE_BERRY, SITE_MAGIC_ESSENCE }, // GRASS
            { SITE_NOTHING, SITE_DEER,    SITE_GOLD,   SITE_STONE, SITE_TREE, SITE_BERRY, SITE_MAGIC_ESSENCE }, // SNOW
            { SITE_NOTHING, SITE_DEER,    SITE_GOLD,   SITE_STONE, SITE_TREE, SITE_BERRY, SITE_MAGIC_ESSENCE }, // ROCK
            { SITE_NOTHING, SITE_DEER,    SITE_GOLD,   SITE_STONE, SITE_TREE, SITE_BERRY, SITE_MAGIC_ESSENCE }, // DESERT
            { SITE_NOTHING, SITE_FISH }                                                                         // WATER
    };

    static const std::vector<std::vector<double>> SITES_DISTRIBUTION_WEIGHTS = {
            { 88.150, 0.50, 0.25, 0.30, 10.000, 0.750, 0.05 },
            { 95.600, 0.10, 0.50, 0.75,  2.000, 0.050, 1.00 },
            { 82.965, 0.02, 5.0,  10.0,  0.010, 0.005, 2.00 },
            { 87.487, 0.01, 2.5,   5.0,  0.002, 0.001, 5.00 },
            { 99.0,   1.0 }
    };

    terra_site_distrib_table* table = terra_site_distrib_table_create();

    for(std::size_t i = 0; i < SITES_DISTRIBUTION_BIOMES.size(); ++i) {
        terra_site_distrib_table_set(table, SITES_DISTRIBUTION_BIOMES[i],
                                     SITES_DISTRIBUTION_SITES[i].size(),
                                     &SITES_DISTRIBUTION_SITES[i][0],
                                     &SITES_DISTRIBUTION_WEIGHTS[i][0]);
    }

    return table;
}

void world_generator::setup_resources() {
    terra_site_distrib_table* distrib_table = make_site_distribution();

    terra_site_generator* site_generator = terra_site_generator_create(TERRA_SITE_GENERATOR_WEIGHT);
    terra_site_generator_set_distrib_table(site_generator, TERRA_SITEGEN_DISTRIBUTION, distrib_table);

    terra_map_set_site_generator(map_generator, site_generator);

    terra_site_generator_destroy(site_generator);
    terra_site_distrib_table_destroy(distrib_table);
}