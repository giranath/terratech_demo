#ifndef DEF_PROFILIER_HPP
#define DEF_PROFILIER_HPP

#include "profiler_administrator.hpp"
#include <chrono>
#include <string>

template <class T>
class profiler
{
    static_assert(is_time<T>::value, "type must be of time");

    std::chrono::steady_clock::time_point begin;
    std::string name;
public:
    profiler(const std::string& name): 
        begin(std::chrono::high_resolution_clock::now()),
        name(name)
    {
    }
    ~profiler()
    {
        std::chrono::steady_clock::time_point end = std::chrono::high_resolution_clock::now();
        profiler_administrator<T>::get_instance().log_time(name, begin, end);
    }
};
#endif