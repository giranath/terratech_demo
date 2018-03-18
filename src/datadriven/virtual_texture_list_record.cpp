#include "virtual_texture_list_record.hpp"

namespace data {

std::istream &operator>>(std::istream &stream, rendering::virtual_texture::area_type &area) {
    rendering::virtual_texture::area_type::value_type left, right, top, bottom;
    stream >> left >> bottom >> right >> top;

    area = rendering::virtual_texture::area_type(left, bottom, right, top);

    return stream;
}

std::istream &operator>>(std::istream &stream, virtual_texture_list_record &record) {
    return stream >> record.name >> record.id >> record.texture_id >> record.area;
}

}