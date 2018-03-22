#include "world.hpp"
#include <algorithm>
#include <iterator>

world_chunk* world::chunk_at(int x, int z) {
    auto it = std::find_if(std::begin(chunks), std::end(chunks), [x, z](const world_chunk& chunk) {
        return chunk.position() == world_chunk::position_type{x, z};
    });

    if(it == std::end(chunks)) return nullptr;

    return &(*it);
}

const world_chunk* world::chunk_at(int x, int z) const {
    auto it = std::find_if(std::begin(chunks), std::end(chunks), [x, z](const world_chunk& chunk) {
        return chunk.position() == world_chunk::position_type{x, z};
    });

    if(it == std::end(chunks)) return nullptr;

    return &(*it);
}

world_chunk& world::add(int x, int z) {
    chunks.emplace_back(x, z);

    return chunks.back();
}

world::iterator world::begin() {
    return chunks.begin();
}

world::iterator world::end() {
    return chunks.end();
}

world::const_iterator world::begin() const {
    return chunks.begin();
}

world::const_iterator world::end() const {
    return chunks.end();
}

bool world::has_chunk(int x, int z) const noexcept {
    return std::find_if(begin(), end(), [x, z](const world_chunk& chunk) {
        return chunk.position() == glm::i32vec2{x, z};
    }) != end();
}