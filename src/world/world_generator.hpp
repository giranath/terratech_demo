#ifndef MMAP_DEMO_WORLD_GENERATOR_H
#define MMAP_DEMO_WORLD_GENERATOR_H

#include <terratech/terratech.h>
#include <cstdint>

class invalid_layer_exception{};

class world_generator {
    terra_map* map_generator;
    terra_layer_handle altitude_layer;
    terra_layer_handle humidity_layer;
    terra_layer_handle temperature_layer;

    static constexpr float CHUNK_NOISE_RATIO = 0.25f;

    void setup_biomes();
    void setup_resources();

public:
    world_generator(uint32_t seed, uint32_t chunk_width, uint32_t chunk_depth);
    ~world_generator();
};


#endif
