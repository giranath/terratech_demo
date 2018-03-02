#include "world_chunk.hpp"
#include "world.hpp"

world_chunk::world_chunk(int x, int z) {

}

void world_chunk::load(terra_chunk* chunk) noexcept {
    biomes.reserve(world::CHUNK_WIDTH * world::CHUNK_HEIGHT * world::CHUNK_DEPTH);

    for(std::size_t y = 0; y < world::CHUNK_HEIGHT; ++y) {
        for(std::size_t z = 0; z < world::CHUNK_DEPTH; ++z) {
            for(std::size_t x = 0; x < world::CHUNK_WIDTH; ++x) {
                biomes.push_back(terra_chunk_biome_at(chunk, x, y, z));
            }
        }
    }
    // TODO: Iterate on chunk
}

int world_chunk::biome_at(int x, int y, int z) const noexcept {
    const std::size_t Y_OFFSET = world::CHUNK_WIDTH * world::CHUNK_DEPTH;
    const std::size_t Z_OFFSET = world::CHUNK_WIDTH;
    return biomes[y * Y_OFFSET + z * Z_OFFSET + x];
}