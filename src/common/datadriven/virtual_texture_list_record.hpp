#ifndef MMAP_DEMO_VIRTUAL_TEXTURE_LIST_RECORD_HPP
#define MMAP_DEMO_VIRTUAL_TEXTURE_LIST_RECORD_HPP

#include "../bounding_box.hpp"
#include <string>
#include <iostream>

namespace data {

struct virtual_texture_list_record {
    std::string name;
    int id;
    int texture_id;
    bounding_box<float> area;
};

std::istream &operator>>(std::istream &stream, virtual_texture_list_record &record);

}


#endif //MMAP_DEMO_VIRTUAL_TEXTURE_LIST_RECORD_HPP
