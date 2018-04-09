#include "world_renderer.hpp"
#include "../debug/profiler.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace rendering {

world_renderer::chunk_rendering::chunk_rendering(const world_chunk &chunk)
: renderer{chunk}, pos{chunk.position()}, is_visible{true} {

};

world_renderer::world_renderer(world &w) noexcept
: w{w} {
    chunk_renderers.reserve(20 * 20);
}

void world_renderer::show(int x, int z) noexcept {
    auto it = std::find_if(std::begin(chunk_renderers), std::end(chunk_renderers), [x, z](const chunk_rendering& r) {
       return r.pos.x == x && r.pos.y == z;
    });

    if(it != std::end(chunk_renderers)) {
        it->is_visible = true;
    }
    else {
        chunk_renderers.emplace_back(*w.chunk_at(x, z));
    }
}

void world_renderer::hide_all() noexcept {
    std::for_each(std::begin(chunk_renderers), std::end(chunk_renderers), [](chunk_rendering& r) {
       r.is_visible = false;
    });
}

void world_renderer::render(mesh_rendering_system &rendering, glm::mat4 parent_model) noexcept {
    std::for_each(std::begin(chunk_renderers), std::end(chunk_renderers),
                  [parent_model, &rendering](const chunk_rendering &renderer) {
                      glm::mat4 model = glm::translate(parent_model, glm::vec3{
                              renderer.pos.x * chunk_renderer::SQUARE_SIZE * world::CHUNK_WIDTH,
                              0.f,
                              renderer.pos.y * chunk_renderer::SQUARE_SIZE * world::CHUNK_DEPTH});


                      if(renderer.is_visible) {
                        renderer.renderer.render(rendering, model);
                      }
                  });
}

world_renderer::iterator world_renderer::begin() {
    return chunk_renderers.begin();
}

world_renderer::iterator world_renderer::end() {
    return chunk_renderers.end();
}

world_renderer::const_iterator world_renderer::begin() const {
    return chunk_renderers.begin();
}

world_renderer::const_iterator world_renderer::end() const {
    return chunk_renderers.end();
}

}