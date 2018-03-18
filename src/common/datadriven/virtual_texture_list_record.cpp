#include "virtual_texture_list_record.hpp"

namespace data {

std::istream &operator>>(std::istream &stream, bounding_box<float>& area) {
    float left, right, top, bottom;
    stream >> left >> bottom >> right >> top;

    area = bounding_box<float>(left, bottom, right, top);

    return stream;
}

std::istream &operator>>(std::istream &stream, virtual_texture_list_record &record) {
    return stream >> record.name >> record.id >> record.texture_id >> record.area;
}

}