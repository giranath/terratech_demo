#ifndef MMAP_DEMO_VIRTUAL_TEXTURE_HPP
#define MMAP_DEMO_VIRTUAL_TEXTURE_HPP

#include "../opengl/texture.hpp"
#include "../bounding_box.hpp"

namespace rendering {

class virtual_texture {
public:
    using area_type = bounding_box<float>;
private:
    gl::texture* texture_;
    area_type area_;

public:
    virtual_texture() noexcept;
    explicit virtual_texture(gl::texture& texture) noexcept;
    virtual_texture(gl::texture& texture, area_type area) noexcept;
    virtual_texture(virtual_texture&& texture) noexcept;

    virtual_texture& operator=(virtual_texture&& other) noexcept;

    bool is_bound() const noexcept;

    const gl::texture& texture() const noexcept;
    const area_type& area() const noexcept;
};

}

#endif //MMAP_DEMO_VIRTUAL_TEXTURE_HPP
