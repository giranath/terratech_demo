#ifndef MMAP_DEMO_WORLD_RENDERER_HPP
#define MMAP_DEMO_WORLD_RENDERER_HPP

#include "../opengl/opengl.hpp"
#include "chunk_renderer.hpp"
#include "../../common/world/world.hpp"
#include "rendering_manager.hpp"
#include <vector>

namespace rendering {

class world_renderer {
    world &w;

    struct chunk_rendering {
        chunk_renderer renderer;
        world_chunk::position_type pos;

        chunk_rendering(const world_chunk &chunk);
    };

    std::vector<chunk_rendering> chunk_renderers;
public:
    world_renderer(world &w) noexcept;

    void show(int x, int z) noexcept;

    void render(mesh_rendering_system &rendering, glm::mat4 parent_model = glm::mat4{1.f}) noexcept;
};

}


#endif
