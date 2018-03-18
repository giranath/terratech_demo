#ifndef MMAP_DEMO_SHADER_LIST_RECORD_HPP
#define MMAP_DEMO_SHADER_LIST_RECORD_HPP

#include <string>
#include <iostream>

namespace data {
struct shader_list_record {
    int id;
    std::string vertex_path;
    std::string fragment_path;
};

std::istream& operator>>(std::istream& stream, shader_list_record& record);

}


#endif //MMAP_DEMO_SHADER_LIST_RECORD_HPP
