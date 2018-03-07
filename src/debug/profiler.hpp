#ifndef DEF_PROFILIER_HPP
#define DEF_PROFILIER_HPP

#include "profiler_administrator.hpp"
#include <chrono>
#include <string>

template <class T, typename Clock = std::chrono::high_resolution_clock>
class profiler
{
    using time_point = typename Clock::time_point;
    static_assert(is_time<T>::value, "type must be of time");

    time_point begin;
    std::string name;
public:
    profiler(const std::string& name): 
        begin(Clock::now()),
        name(name)
    {
    }
    ~profiler()
    {
        time_point end = Clock::now();
        profiler_administrator<T>::get_instance().log_time(name, begin, end);
    }
};
#endif