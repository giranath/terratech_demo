#ifndef MMAP_DEMO_WORLD_H
#define MMAP_DEMO_WORLD_H

#include "world_generator.hpp"
#include "world_chunk.hpp"
#include <cstdint>
#include <vector>

class world {
    world_generator generator;
    std::vector<world_chunk> chunks;
public:
    static const uint32_t CHUNK_WIDTH = 32;
    static const uint32_t CHUNK_HEIGHT = 1;
    static const uint32_t CHUNK_DEPTH = 32;

    explicit world(uint32_t seed);

    world_chunk& generate_at(int x, int z);
};

#endif
