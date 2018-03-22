#include "world.hpp"

#include <algorithm>
#include <iterator>

infinite_world::infinite_world(uint32_t seed)
: generator(seed, CHUNK_WIDTH, CHUNK_DEPTH){

}

world_chunk& infinite_world::generate_at(int x, int z) noexcept {
    world_chunk& chunk = add(x, z);

    auto generated_chunk = generator.generate_chunk(x, 0, z);
    chunk.load(generated_chunk);

    return chunk;
}

world_chunk* infinite_world::chunk_at(int x, int z) {
    auto it = std::find_if(begin(), end(), [x, z](const world_chunk& chunk) {
        return chunk.position() == world_chunk::position_type{x, z};
    });

    if(it == end()) {
        return &generate_at(x, z);
    }

    return &(*it);
}

const world_chunk* infinite_world::chunk_at(int x, int z) const {
    auto it = std::find_if(begin(), end(), [x, z](const world_chunk& chunk) {
        return chunk.position() == world_chunk::position_type{x, z};
    });

    if(it == end()) {
        return nullptr;
    }

    return &(*it);
}