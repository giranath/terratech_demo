#ifndef MMAP_DEMO_WORLD_CHUNK_H
#define MMAP_DEMO_WORLD_CHUNK_H

#include "site.hpp"
#include "constants.hpp"

#include <terratech/terratech.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

template<typename T>
struct vec3_hash {
    std::size_t operator()(const T& vec) const {
        using val_type = typename T::value_type;
        return ((std::hash<val_type>()(vec.x)
                 ^ (std::hash<val_type>()(vec.y) << 1)) >> 1)
               ^ (std::hash<val_type>()(vec.z) << 1);
    }
};

template<typename T>
struct vec2_hash {
std::size_t operator()(const T& vec) const {
    using val_type = typename T::value_type;
    return ((std::hash<val_type>()(vec.x)
             ^ (std::hash<val_type>()(vec.y) << 1)) >> 1);
}
};

class world_chunk {
public:
    using position_type = glm::i32vec2;
private:
    std::vector<int> biomes;
    const position_type pos;
    std::unordered_map<glm::i32vec3, std::vector<site>, vec3_hash<glm::i32vec3>> sites;

    static std::unordered_map<int, double> site_scores();
    static std::unordered_map<int, double> biome_scores();
public:
    world_chunk(int x, int z);
    /**
     * Load the chunk from terratech
     * @param chunk The terratech chunk to load
     */
    void load(terra_chunk* chunk) noexcept;
    void set_biome_at(const std::vector<uint8_t>& biome_vec) noexcept;
    void set_biome_at(std::vector<uint8_t>&& biome_vec) noexcept;
    int biome_at(int x, int y, int z) const noexcept;
    std::vector<site*> sites_at(int x, int y, int z) noexcept;
    std::vector<const site*> sites_at(int x, int y, int z) const noexcept;

    position_type position() const noexcept;

    double score() const noexcept;
};

#endif //MMAP_DEMO_WORLD_CHUNK_H
