#ifndef MMAP_DEMO_WORLD_CHUNK_H
#define MMAP_DEMO_WORLD_CHUNK_H

#include <terratech/terratech.h>
#include <vector>

class world_chunk {
    std::vector<int> biomes;
public:
    world_chunk(int x, int z);
    /**
     * Load the chunk from terratech
     * @param chunk The terratech chunk to load
     */
    void load(terra_chunk* chunk) noexcept;
};

#endif //MMAP_DEMO_WORLD_CHUNK_H
