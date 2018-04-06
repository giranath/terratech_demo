#ifndef MMAP_DEMO_WORLD_GENERATOR_H
#define MMAP_DEMO_WORLD_GENERATOR_H

#include <terratech/terratech.h>
#include <cstdint>
#include "constants.hpp"

class invalid_layer_exception{};

class world_generator;
enum class map_choice
{
    RIVER_MAP,
    ISLAND_MAP,
    LAKE_MAP,
    PLAIN_MAP
};

struct circle {
    double x;
    double y;
    double radius;
    circle() = default;
    circle(double x, double y, double radius) :
        x{ x },
        y{ y },
        radius{ radius } {

    }
};

struct oval
{
    double cx;
    double cy;
    double height_radius;
    double width_radius;
    oval() = default;
    oval(double x, double y, double height_radius, double width_radius) : cx{ x }, cy{ y }, height_radius{ height_radius }, width_radius{ width_radius } {

    }
    bool is_inside(double x, double y)
    {
        double inside = ((x - cx) * (x - cx)) / (width_radius * width_radius) + ((y - cy) * (y - cy)) / (height_radius * height_radius);
        if (inside <= 1.0)
            return true;
        return false;
    }
};


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
    terra_layer_handle chosen_altitude_layer;
    terra_layer_handle humidity_layer;
    terra_layer_handle temperature_layer;

    union user_data
    {
        circle island;
        circle lake;
        oval player_island[2];
    };
    user_data data_user;

    map_choice chosen_map;
    static constexpr float CHUNK_NOISE_RATIO = 0.25f;

    void setup_biomes();
    void setup_resources();

public:
    world_generator(uint32_t seed, uint32_t chunk_width, uint32_t chunk_depth, map_choice map_choice);
    ~world_generator();
    void setup_river_map_layer();
    void setup_island_map_layer();
    void setup_lake_map_layer();
    void setup_plain_map_layer();
    world_generator_chunk generate_chunk(int x, int y, int z) const noexcept;
};


#endif
