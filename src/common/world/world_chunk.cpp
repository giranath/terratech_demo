#include "world_chunk.hpp"
#include "world.hpp"

#include <algorithm>
#include <iterator>

world_chunk::world_chunk(int x, int z)
: pos{x, z}{

}

void world_chunk::load(terra_chunk* chunk) noexcept {
    biomes.reserve(world::CHUNK_WIDTH * world::CHUNK_HEIGHT * world::CHUNK_DEPTH);

    for(std::size_t y = 0; y < world::CHUNK_HEIGHT; ++y) {
        for(std::size_t z = 0; z < world::CHUNK_DEPTH; ++z) {
            for(std::size_t x = 0; x < world::CHUNK_WIDTH; ++x) {
                biomes.push_back(terra_chunk_biome_at(chunk, x, y, z));

                // Fetch the raw sites
                std::vector<int> raw_sites;
                std::size_t site_counts = terra_chunk_sites_count_at(chunk, x, y, z);
                raw_sites.resize(site_counts);
                terra_chunk_sites_at(chunk, x, y, z, &raw_sites.front(), raw_sites.size());

                std::vector<site> sites;
                sites.reserve(raw_sites.size());
                std::transform(std::begin(raw_sites), std::end(raw_sites), std::back_inserter(sites), [](int id) {
                    // TODO: Initializes the site with a factory
                    return site(static_cast<site::id>(id), 100);
                });

                this->sites[glm::i32vec3{x, y, z}] = std::move(sites);
            }
        }
    }
}

void world_chunk::set_biome_at(const std::vector<uint8_t>& biome_vec) noexcept
{
    std::copy(biome_vec.begin(), biome_vec.end(), biomes.begin());
}

void world_chunk::set_biome_at(std::vector<uint8_t>&& biome_vec) noexcept
{
    biomes.resize(biome_vec.size());
    std::copy(biome_vec.begin(), biome_vec.end(), biomes.begin());
}

int world_chunk::biome_at(int x, int y, int z) const noexcept {
    const std::size_t Y_OFFSET = world::CHUNK_WIDTH * world::CHUNK_DEPTH;
    const std::size_t Z_OFFSET = world::CHUNK_WIDTH;
    return biomes[y * Y_OFFSET + z * Z_OFFSET + x];
}

std::vector<site*> world_chunk::sites_at(int x, int y, int z) noexcept {
    std::vector<site*> ret_sites;

    auto it = sites.find(glm::i32vec3{x, y, z});
    if(it != sites.end()) {
        std::transform(std::begin(it->second), std::end(it->second), std::back_inserter(ret_sites), [](site& s) {
           return &s;
        });
    }

    return ret_sites;
}

std::vector<const site*> world_chunk::sites_at(int x, int y, int z) const noexcept {
    std::vector<const site*> ret_sites;

    auto it = sites.find(glm::i32vec3{x, y, z});
    if(it != sites.end()) {
        std::transform(std::begin(it->second), std::end(it->second), std::back_inserter(ret_sites), [](const site& s) {
            return &s;
        });
    }

    return ret_sites;
}

world_chunk::position_type world_chunk::position() const noexcept {
    return pos;
}