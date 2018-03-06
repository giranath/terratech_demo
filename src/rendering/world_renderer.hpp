#ifndef MMAP_DEMO_WORLD_RENDERER_HPP
#define MMAP_DEMO_WORLD_RENDERER_HPP

#include "../opengl/opengl.hpp"
#include "chunk_renderer.hpp"
#include "../world/world.hpp"
#include <vector>

class world_renderer {
    world& w;

    struct chunk_rendering {
        chunk_renderer renderer;
        world_chunk::position_type pos;

        chunk_rendering(const world_chunk& chunk);
    };
    std::vector<chunk_rendering> chunk_renderers;
public:
    world_renderer(world& w) noexcept;

    void show(int x, int z) noexcept;

    void render(gl::program& program) noexcept;
};


#endif
