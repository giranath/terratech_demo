#include "virtual_texture.hpp"

namespace rendering {

virtual_texture::virtual_texture() noexcept
: texture_(nullptr)
, area_(0.f, 0.f, 1.f, 1.f) {

}

virtual_texture::virtual_texture(gl::texture& texture) noexcept
: texture_(&texture)
, area_(0.f, 0.f, 1.f, 1.f) {

}

virtual_texture::virtual_texture(gl::texture& texture, area_type area) noexcept
: texture_(&texture)
, area_(area) {

}

virtual_texture::virtual_texture(virtual_texture&& texture) noexcept
: texture_(texture.texture_)
, area_(std::move(texture.area_)) {

}

virtual_texture& virtual_texture::operator=(virtual_texture&& other) noexcept {
    texture_ = other.texture_;
    area_ = std::move(other.area_);

    return *this;
}

bool virtual_texture::is_bound() const noexcept {
    return texture_ != nullptr;
}

const gl::texture& virtual_texture::texture() const noexcept {
    return *texture_;
}

const virtual_texture::area_type& virtual_texture::area() const noexcept {
    return area_;
}

}