#include "world_renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

world_renderer::chunk_rendering::chunk_rendering(const world_chunk& chunk)
: renderer{chunk}
, pos{chunk.position()}{

};

world_renderer::world_renderer(world& w, gl::texture& terrain_texture) noexcept
: w{w}
, terrain_texture(terrain_texture){

}

void world_renderer::show(int x, int z) noexcept {
    chunk_renderers.emplace_back(w.chunk_at(x, z));
}

void world_renderer::render(gl::program& program, glm::mat4 parent_model) noexcept {
    gl::uniform<glm::mat4> model_uniform = program.find_uniform<glm::mat4>("model_matrix");
    gl::uniform<int> is_textured = program.find_uniform<int>("is_textured");
    is_textured.set(1);

    glActiveTexture(GL_TEXTURE0);
    gl::bind(gl::texture_bind<GL_TEXTURE_2D>(terrain_texture));

    std::for_each(std::begin(chunk_renderers), std::end(chunk_renderers), [&model_uniform, &program, parent_model](const chunk_rendering& renderer) {
        glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{renderer.pos.x * chunk_renderer::SQUARE_SIZE * world::CHUNK_WIDTH,
                                                                   0.f,
                                                                   renderer.pos.y * chunk_renderer::SQUARE_SIZE * world::CHUNK_DEPTH});

        model = parent_model * model;
        model_uniform.set(model);

        renderer.renderer.render(program, model);
    });
}