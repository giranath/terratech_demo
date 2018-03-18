#include "shader_list_record.hpp"

namespace data {

std::istream& operator>>(std::istream& stream, shader_list_record& record) {
    return stream >> record.id >> record.vertex_path >> record.fragment_path;
}

}