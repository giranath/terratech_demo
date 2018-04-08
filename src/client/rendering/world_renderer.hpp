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
public:
    struct chunk_rendering {
        chunk_renderer renderer;
        world_chunk::position_type pos;
        bool is_visible;

        chunk_rendering(const world_chunk &chunk);
    };
private:
    std::vector<chunk_rendering> chunk_renderers;
public:
    using iterator = std::vector<chunk_rendering>::iterator;
    using const_iterator = std::vector<chunk_rendering>::const_iterator;

    world_renderer(world &w) noexcept;

    void show(int x, int z) noexcept;
    void hide_all() noexcept;

    void render(mesh_rendering_system &rendering, glm::mat4 parent_model = glm::mat4{1.f}) noexcept;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
};

}


#endif
