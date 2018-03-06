#include "world_renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

world_renderer::chunk_rendering::chunk_rendering(const world_chunk& chunk)
: renderer{chunk}
, pos{chunk.position()}{

};

world_renderer::world_renderer(world& w) noexcept
: w{w} {

}

void world_renderer::show(int x, int z) noexcept {
    chunk_renderers.emplace_back(w.chunk_at(x, z));
}

void world_renderer::render(gl::program& program) noexcept {
    gl::uniform<glm::mat4> model_uniform = program.find_uniform<glm::mat4>("model_matrix");

    std::for_each(std::begin(chunk_renderers), std::end(chunk_renderers), [&model_uniform](const chunk_rendering& renderer) {
        glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{renderer.pos.x * chunk_renderer::SQUARE_SIZE * world::CHUNK_WIDTH,
                                                                   0.f,
                                                                   renderer.pos.y * chunk_renderer::SQUARE_SIZE * world::CHUNK_DEPTH});
        model_uniform.set(model);

        renderer.renderer.render();
    });
}