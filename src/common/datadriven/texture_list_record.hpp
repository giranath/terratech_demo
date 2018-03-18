#ifndef MMAP_DEMO_TEXTURE_LIST_RECORD_HPP
#define MMAP_DEMO_TEXTURE_LIST_RECORD_HPP

#include <string>
#include <iostream>

namespace data {

struct texture_list_record {
    int id;
    std::string path;
};

std::istream& operator>>(std::istream& stream, texture_list_record& record);

}

#endif //MMAP_DEMO_TEXTURE_LIST_RECORD_HPP
