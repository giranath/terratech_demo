#include "world.hpp"

world::world(uint32_t seed)
: generator(seed, CHUNK_WIDTH, CHUNK_DEPTH){

}

world_chunk& world::generate_at(int x, int z) {
    chunks.emplace_back(x, z);

    world_chunk& chunk = chunks.back();

    auto generated_chunk = generator.generate_chunk(x, 0, z);
    chunk.load(generated_chunk);

    return chunk;
}