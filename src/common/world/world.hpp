#ifndef MMAP_DEMO_WORLD_H
#define MMAP_DEMO_WORLD_H

#include "world_generator.hpp"
#include "world_chunk.hpp"
#include <cstdint>
#include <vector>

class world {
public:
    using chunk_collection = std::vector<world_chunk>;
    using iterator = chunk_collection::iterator;
    using const_iterator = chunk_collection::const_iterator;
private:
    chunk_collection chunks;
public:
    static const uint32_t CHUNK_WIDTH = 16;
    static const uint32_t CHUNK_HEIGHT = 1;
    static const uint32_t CHUNK_DEPTH = 16;

    virtual world_chunk* chunk_at(int x, int z);
    virtual const world_chunk* chunk_at(int x, int z) const;

    world_chunk& add(int x, int z);
    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    bool has_chunk(int x, int z) const noexcept;
};

class infinite_world : public world {
    world_generator generator;
public:
    explicit infinite_world(uint32_t seed);

    world_chunk& generate_at(int x, int z) noexcept;

    world_chunk* chunk_at(int x, int z) override;
    const world_chunk* chunk_at(int x, int z) const override;

    bool is_chunk_generated(int x, int z) const noexcept;
};

#endif
