#include "world_chunk.hpp"
#include "world.hpp"

#include <algorithm>
#include <iterator>
#include <numeric>

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
    biomes.resize(biome_vec.size());
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

std::unordered_map<int, double> world_chunk::site_scores() {
    std::unordered_map<int, double> scores;
    scores[SITE_TREE] = 10.0;
    scores[SITE_BERRY] = 8.0;
    scores[SITE_DEER] = 8.0;
    scores[SITE_STONE] = 4.0;
    scores[SITE_MAGIC_ESSENCE] = 4.0;
    scores[SITE_GOLD] = 4.0;
    scores[SITE_NOTHING] = 0.0;
    scores[SITE_FISH] = 0.0;

    return scores;
}

std::unordered_map<int, double> world_chunk::biome_scores() {
    std::unordered_map<int, double> scores;
    scores[BIOME_GRASS] = 10.0;
    scores[BIOME_ROCK] = 5.0;
    scores[BIOME_DESERT] = 2.0;
    scores[BIOME_WATER] = 0.0;
    scores[BIOME_SNOW] = 2.0;

    return scores;
}

double world_chunk::score() const noexcept {
    const auto& sites_s = site_scores();
    const auto& biomes_s = biome_scores();

    double score = std::accumulate(std::begin(biomes), std::end(biomes), 0.0, [&biomes_s](double current, int biome) {
        return current + biomes_s.at(biome);
    });

    score = std::accumulate(std::begin(sites), std::end(sites), score, [&sites_s](double current, auto pair) {
        if(pair.second.empty()) {
            return current;
        }
        return current + sites_s.at(pair.second.front().type());
    });

    return score;
}