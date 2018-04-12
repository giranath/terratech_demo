#ifndef MMAP_DEMO_WORLD_RENDERER_HPP
#define MMAP_DEMO_WORLD_RENDERER_HPP

#include "../opengl/opengl.hpp"
#include "chunk_renderer.hpp"
#include "../../common/world/world.hpp"
#include "rendering_manager.hpp"
#include <vector>
#include <array>

namespace rendering {

class world_renderer {
    world &w;
public:
    struct chunk_rendering {
        chunk_renderer renderer;
        world_chunk::position_type pos;
        bool is_visible;

        chunk_rendering(const world_chunk &chunk);
        chunk_rendering(const world_chunk& chunk, gl::buffer&& vertices, gl::buffer&& colors, gl::buffer&& uvs,
                                                  gl::buffer&& svertices, gl::buffer&& scolors, gl::buffer&& suvs);
    };
private:
    static const std::size_t MAX_BUFFER_SIZE = 20 * 20;
    std::vector<chunk_rendering> chunk_renderers;
    std::array<gl::buffer, MAX_BUFFER_SIZE * 2> vertex_buffers;
    std::array<gl::buffer, MAX_BUFFER_SIZE * 2> color_buffers;
    std::array<gl::buffer, MAX_BUFFER_SIZE * 2> uv_buffers;
    std::size_t next_buffer_index;
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
