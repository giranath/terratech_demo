#ifndef MMAP_DEMO_WORLD_CHUNK_H
#define MMAP_DEMO_WORLD_CHUNK_H

#include <terratech/terratech.h>
#include <glm/glm.hpp>
#include <vector>

class world_chunk {
public:
    using position_type = glm::i32vec2;
private:
    std::vector<int> biomes;
    const position_type pos;
public:
    world_chunk(int x, int z);
    /**
     * Load the chunk from terratech
     * @param chunk The terratech chunk to load
     */
    void load(terra_chunk* chunk) noexcept;

    int biome_at(int x, int y, int z) const noexcept;

    position_type position() const noexcept;
};

#endif //MMAP_DEMO_WORLD_CHUNK_H
