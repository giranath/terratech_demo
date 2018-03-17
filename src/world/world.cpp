#include "world.hpp"
#include <algorithm>
#include <iterator>

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

world_chunk& world::chunk_at(int x, int z) {
    auto it = std::find_if(std::begin(chunks), std::end(chunks), [x, z](const world_chunk& chunk) {
        return chunk.position() == world_chunk::position_type{x, z};
    });

    if(it == std::end(chunks)) {
        return generate_at(x, z);
    }

    return *it;
}

world_chunk* world::get_chunk(int x, int z) {
    auto it = std::find_if(std::begin(chunks), std::end(chunks), [x, z](const world_chunk& chunk) {
        return chunk.position() == world_chunk::position_type{x, z};
    });

    if(it == std::end(chunks)) return nullptr;

    return &(*it);
}

const world_chunk* world::get_chunk(int x, int z) const {
    auto it = std::find_if(std::begin(chunks), std::end(chunks), [x, z](const world_chunk& chunk) {
        return chunk.position() == world_chunk::position_type{x, z};
    });

    if(it == std::end(chunks)) return nullptr;

    return &(*it);
}