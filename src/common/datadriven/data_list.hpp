#ifndef MMAP_DEMO_DATA_LIST_HPP
#define MMAP_DEMO_DATA_LIST_HPP

#include <vector>
#include <algorithm>
#include <iterator>

namespace data {

template<typename Type, typename Fn>
std::vector<Type> load_data_list(std::istream& stream, Fn fn) {
    std::vector<Type> elements;
    std::copy(std::istream_iterator<Type>(stream), std::istream_iterator<Type>(), std::back_inserter(elements));

    std::for_each(std::begin(elements), std::end(elements), fn);

    return elements;
}

}

#endif //MMAP_DEMO_DATA_LIST_HPP
