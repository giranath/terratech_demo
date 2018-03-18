#include "texture_list_record.hpp"

namespace data {

std::istream& operator>>(std::istream& stream, texture_list_record& record) {
    return stream >> record.id >> record.path;
}

}