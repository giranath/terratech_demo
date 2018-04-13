#include "world_generator.hpp"

#include <vector>
#include <iostream>
#include <cmath>

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

world_generator::world_generator(uint32_t seed, uint32_t chunk_width, uint32_t chunk_depth, map_choice chosen_map)
: map_generator{terra_map_create(seed, chunk_width, 1, chunk_depth,  static_cast<double>(chunk_width) / (static_cast<double>(chunk_width) * 20))}
, altitude_layer{terra_map_add_noise_layer(map_generator, TERRA_NOISE_OCTAVE_PERLIN)}
, humidity_layer{terra_map_add_noise_layer(map_generator, TERRA_NOISE_OCTAVE_PERLIN)}
, temperature_layer{terra_map_add_noise_layer(map_generator, TERRA_NOISE_OCTAVE_PERLIN)
, }{
    if(altitude_layer == TERRA_LAYER_INVALID
    || humidity_layer == TERRA_LAYER_INVALID
    || temperature_layer == TERRA_LAYER_INVALID) {
        throw invalid_layer_exception{};
    }
	terra_layer_set_property_double(terra_map_get_layer(map_generator, humidity_layer), TERRA_NOISE_SCALE, 4.0);
	terra_layer_set_property_double(terra_map_get_layer(map_generator, temperature_layer), TERRA_NOISE_SCALE, 4.0);

    switch (chosen_map)
    {
    case map_choice::ISLAND_MAP :
        setup_island_map_layer();
        break;
    case map_choice::LAKE_MAP:
        setup_lake_map_layer();
        break;
    case map_choice::PLAIN_MAP :
        setup_plain_map_layer();
        break;
    case map_choice::RIVER_MAP :
        setup_river_map_layer();
        break;
    default :
        setup_plain_map_layer();
        break;
    }
    setup_biomes();
    setup_resources();
}

world_generator::~world_generator() {
    terra_map_destroy(map_generator);
}

world_generator_chunk world_generator::generate_chunk(int x, int /*y*/, int z) const noexcept {
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
    terra_biome_table* altitude_axis = terra_biome_table_create(20, 0);

    terra_biome_table_set(altitude_axis, 0,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 1,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 2,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 3,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 4,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 5,  0, BIOME_WATER);
    terra_biome_table_set(altitude_axis, 6,  0, BIOME_WATER);
    return altitude_axis;
}

double circle_custom_noise(double x, double /*y*/, double z, terra_userdata userdata) {
    circle* island = static_cast<circle*>(userdata);
    if (x > 0.5)
    {
        int ok = 0;
    }
    double distance = std::sqrt((x - island->x) * (x - island->x) + (z - island->y) * (z - island->y));
    if (distance > island->radius) {
        return 0.0;
    }

    double percent = distance / island->radius;
    return 1.0 - percent;
}

double inverse_circle_custom_noise(double x, double /*y*/, double z, terra_userdata userdata) {
    circle* island = static_cast<circle*>(userdata);

    double distance = std::sqrt((x - island->x) * (x - island->x) + (z - island->y) * (z - island->y));
    if (distance > island->radius) {
        return 1.0;
    }

    double percent = distance / island->radius;
    return percent;
}

double oval_custom_noise(double x, double /*y*/, double z, terra_userdata userdata) {
    oval* island = static_cast<oval*>(userdata);

    if (!island->is_inside(x, z))
    {
        return 0.0;
    }
    if (x - 0.01 < island->cx  && x + 0.01 > island->cx)
    {
        if (z - 0.01 < island->cy - island->height_radius  && z + 0.01 > island->cy - island->height_radius)
            auto bob = 0;
    }
    double width_to_height_ratio = island->width_radius / island->height_radius;
    double x_dist = ((x - island->cx) * (x - island->cx)) / (island->width_radius * island->width_radius);
    double y_dist = ((z - island->cy) * (z - island->cy)) / (island->height_radius * island->height_radius);
    double x_val = ((x_dist / width_to_height_ratio) * (x_dist / width_to_height_ratio));
    double y_val = (y_dist * y_dist);
    double percent = sqrt(x_val + y_val);

    return 1.0 - percent;
}


double combine_noises(double a, double b, terra_userdata /*userdata*/) {
    if (b > 0.0) {
        return (a * b) * 1.2;
    }

    return 0.0;
}

double lowest_value_combine(double a, double b, terra_userdata)
{
    if (a < b)
    {
        return a;
    }
    return b;
}

double best_value_combine(double a, double b, terra_userdata)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

void world_generator::setup_river_map_layer()
{
    //TODO change scale
    auto river_layer = terra_map_add_noise_layer(map_generator, TERRA_NOISE_OCTAVE_PERLIN);
    auto river_combo = terra_map_add_noise_combinator_layer(map_generator, altitude_layer, river_layer, lowest_value_combine, nullptr);

    data_user.player_island[0] = oval(0.5, 0.1, 0.4, 0.7);
    data_user.player_island[1] = oval(0.5, 0.9, 0.4, 0.7);
    auto oval_island_shape_layer = terra_map_add_custom_noise_layer(map_generator, oval_custom_noise, nullptr, &data_user.player_island[0]);
    auto oval_island_shape_layer_2 = terra_map_add_custom_noise_layer(map_generator, oval_custom_noise, nullptr, &data_user.player_island[1]);

    auto combination_player_1 = terra_map_add_noise_combinator_layer(map_generator, altitude_layer, oval_island_shape_layer, combine_noises, nullptr);
    auto combination_player_2 = terra_map_add_noise_combinator_layer(map_generator, altitude_layer, oval_island_shape_layer_2, combine_noises, nullptr);

    chosen_altitude_layer = terra_map_add_noise_combinator_layer(map_generator, combination_player_1, combination_player_2, best_value_combine, nullptr);
    //chosen_altitude_layer = terra_map_add_noise_combinator_layer(map_generator, combine_both_player_island, river_layer, lowest_value_combine, nullptr);
}

void world_generator::setup_island_map_layer()
{
    //TODO change scale
    auto river_layer = terra_map_add_noise_layer(map_generator, TERRA_NOISE_OCTAVE_PERLIN);
    auto river_combo = terra_map_add_noise_combinator_layer(map_generator, altitude_layer, river_layer, lowest_value_combine, nullptr);

    data_user.island = circle(0.5, 0.5, 0.6);

    auto island_shape_layer = terra_map_add_custom_noise_layer(map_generator, circle_custom_noise, nullptr, &data_user.island);
    chosen_altitude_layer = terra_map_add_noise_combinator_layer(map_generator, altitude_layer, island_shape_layer, combine_noises, nullptr);
    
    //chosen_altitude_layer = terra_map_add_noise_combinator_layer(map_generator, combine_big_island_layer, river_layer, lowest_value_combine, nullptr);
}

void world_generator::setup_lake_map_layer()
{
    auto river_layer = terra_map_add_noise_layer(map_generator, TERRA_NOISE_OCTAVE_PERLIN);
    //auto river_combo = terra_map_add_noise_combinator_layer(map_generator, altitude_layer, river_layer, lowest_value_combine, nullptr);

    data_user.lake = circle(0.5, 0.5, 0.3);
    auto inverse_island_shape_layer = terra_map_add_custom_noise_layer(map_generator, inverse_circle_custom_noise, nullptr, &data_user.lake);
    chosen_altitude_layer = terra_map_add_noise_combinator_layer(map_generator, altitude_layer, inverse_island_shape_layer, combine_noises, nullptr);
    
    //chosen_altitude_layer = terra_map_add_noise_combinator_layer(map_generator, combine_lake_layer, river_layer, lowest_value_combine, nullptr);
}

void world_generator::setup_plain_map_layer()
{
    auto river_layer = terra_map_add_noise_layer(map_generator, TERRA_NOISE_OCTAVE_PERLIN);
    chosen_altitude_layer = terra_map_add_noise_combinator_layer(map_generator, altitude_layer, river_layer, lowest_value_combine, nullptr);
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
    terra_biome_generator_set_layer(generator, TERRA_BIOMEGEN_AXIS_Z_NOISE, terra_map_get_layer(map_generator, chosen_altitude_layer));

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
            { 388.150, 0.50, 0.25, 0.30, 10.000, 0.750, 0.05 },
            { 395.600, 0.10, 0.50, 0.75,  2.000, 0.050, 1.00 },
            { 382.965, 0.02, 5.0,  10.0,  0.010, 0.005, 2.00 },
            { 387.487, 0.01, 2.5,   5.0,  0.002, 0.001, 5.00 },
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