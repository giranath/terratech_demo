#include "world_renderer.hpp"
#include "../debug/profiler.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace rendering {

world_renderer::chunk_rendering::chunk_rendering(const world_chunk &chunk)
: renderer{chunk}, pos{chunk.position()} {

};

world_renderer::world_renderer(world &w) noexcept
: w{w} {

}

void world_renderer::show(int x, int z) noexcept {
    chunk_renderers.emplace_back(*w.chunk_at(x, z));
}

void world_renderer::render(mesh_rendering_system &rendering, glm::mat4 parent_model) noexcept {
    profiler_us p("world_renderer");
    std::for_each(std::begin(chunk_renderers), std::end(chunk_renderers),
                  [parent_model, &rendering](const chunk_rendering &renderer) {
                      glm::mat4 model = glm::translate(parent_model, glm::vec3{
                              renderer.pos.x * chunk_renderer::SQUARE_SIZE * world::CHUNK_WIDTH,
                              0.f,
                              renderer.pos.y * chunk_renderer::SQUARE_SIZE * world::CHUNK_DEPTH});


                      renderer.renderer.render(rendering, model);
                  });
}

}