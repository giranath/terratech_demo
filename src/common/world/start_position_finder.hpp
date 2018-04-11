#ifndef DEF_START_POSITION_FINDER_HPP
#define DEF_START_POSITION_FINDER_HPP

#include "world.hpp"
#include "world_generator.hpp"

#include "glm\glm.hpp"

#include <vector>
#include <array>
#include <cstdint>
#include <random>
#include <optional>
#include <numeric>
#include <algorithm>

struct chunk_value
{
    chunk_value(glm::i32vec2 position, int value) :
        position{position}
        , value{ value } {

    }

    glm::i32vec2 position;
    int value;
};

std::unordered_map<int, int> site_value{ {
    { SITE_BERRY, 1 },
    { SITE_DEER, 1 },
    { SITE_FISH, 1 },
    { SITE_GOLD, 10 },
    { SITE_MAGIC_ESSENCE, 5 },
    { SITE_NOTHING, 0 },
    { SITE_STONE , 5 },
    { SITE_TREE, 1 } } }

std::vector<chunk_value> get_all_chunk_value(world* world_map)
{
    std::vector<chunk_value> chunk_values;
    chunk_values.reserve(std::distance(world_map->begin(), world_map->end()));

    for (auto i = world_map->begin(); i < world_map->end(); ++i)
    {
        int total_value = 0;
        for (uint32_t x = 0; x < world_map->CHUNK_WIDTH; ++x)
        {
            for (uint32_t y = 0; y < world_map->CHUNK_DEPTH; ++x)
            {
                total_value += site_value[i->biome_at(x, 0, y)];
            }
        }
        chunk_values.emplace_back(i->position(), total_value);
    }
    return chunk_values;
}


class start_position_finder
{
    world* world_map;
    std::unordered_map<int, int> site_value;

    bool position_is_free(uint32_t x, uint32_t y, world_chunk* chunk)
    {
        int biome = chunk->biome_at(x, 0, y);
        auto sites  = chunk->sites_at(x, 0, y);

        if (biome != BIOME_WATER || sites.size() > 1 || sites.front()->type() != SITE_NOTHING)
        {
            return false;
        }
        return true;
    }

    std::vector<glm::i32vec2> get_all_x_by_y_free_space(uint32_t x, uint32_t y, world_chunk* chunk)
    {
        const uint32_t width = world_map->CHUNK_WIDTH;
        const uint32_t height = world_map->CHUNK_DEPTH;
        std::vector<glm::i32vec2> free;

        std::array<std::array<bool, height>, width> available;

        for (uint32_t i = 0; i < world_map->CHUNK_WIDTH; ++i)
        {
            for (uint32_t j = 0; j < world_map->CHUNK_DEPTH; ++j)
            {
                available[i][j] = position_is_free(i,j, chunk);
            }
        }

        for (uint32_t i = 0; i < world_map->CHUNK_WIDTH - x; ++i)
        {
            for (uint32_t j = 0; j < world_map->CHUNK_DEPTH - y; ++j)
            {
                bool is_free = true;
                for (uint32_t _x = i; _x < i + x; ++_x)
                {
                    for (uint32_t _y = j; _y < j + y; ++_y)
                    {
                        if (!available[_x][_y])
                        {
                            is_free = false;
                        }
                    }
                }
                if (is_free)
                {
                    free.emplace_back(i, j);
                }
            }
        }
        return free;
    }

    std::optional<glm::i32vec2> get_one_random_free_position(int width, int height, world_chunk* chunk, std::default_random_engine& engine )
    {
        auto &free_pos = get_all_x_by_y_free_space(width, height, chunk);
        if (free_pos.empty())
        {
            return std::nullopt;
        }

        std::shuffle(free_pos.begin(), free_pos.end(), engine);
        return free_pos.front();
    }

    
public:
    start_position_finder(world* world_map) :
        world_map{world_map},
        
    {
    }

    bool is_walkable(int biome_id)
    {
        if (biome_id != BIOME_WATER)
        {
            return true;
        }
        return false;
    }

    int get_walkable_number(world_chunk& chunk)
    {
        int nb_walkable = 0;
        for (size_t x = 0; x < world_map->CHUNK_WIDTH; ++x)
        {
            for (size_t y = 0; y < world_map->CHUNK_DEPTH; ++y)
            {
                auto biome_id = chunk.biome_at(x, 0, y);
                if (is_walkable(biome_id))
                {
                    ++nb_walkable;
                }
            }
        }
        return nb_walkable;
    }
    int manhattan_distance(glm::i32vec2 l, glm::i32vec2 r)
    {
        return std::abs(l.x - r.x) + std::abs(l.y - r.y);
    }
    void create_pleyers_start_position(uint32_t seed)
    {
        std::vector<world_chunk> available_chunk;
        available_chunk.reserve(distance(world_map->begin(), world_map->end()));

        auto is_enough = 3 * ((world_map->CHUNK_DEPTH * world_map->CHUNK_WIDTH) / 4);
        for (auto it = world_map->begin(); it < world_map->end(); ++it)
        {
            auto nb = get_walkable_number(*it);
            auto free_space_for_building = get_all_x_by_y_free_space(4, 4, &(*it));
            if (nb > is_enough && !free_space_for_building.empty())
            {
                available_chunk.push_back(*it);
            }
        }

        int minimum_distance = 4;
        std::default_random_engine engine(seed);
        std::shuffle(available_chunk.begin(), available_chunk.end(), engine);
       
        glm::i32vec2 player1_chunk_position;
        glm::i32vec2 player2_chunk_position;

        for (auto it = available_chunk.begin(); it < available_chunk.end() - 1; ++it)
        {
            for (auto it_ = it + 1; it_ < available_chunk.end(); ++it_)
            {
                int distance = manhattan_distance(it->position(), it_->position());
                if (distance > minimum_distance)
                {
                    player1_chunk_position = it->position();
                    player2_chunk_position = it_->position();
                }
            }
        }
        auto player1_chunk = world_map->chunk_at(player1_chunk_position.x, player1_chunk_position.y);
        auto pos = get_one_random_free_position(4, 4, player1_chunk, engine);

    }
   

    
};
#endif
