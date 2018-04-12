#ifndef DEF_PROFILIER_HPP
#define DEF_PROFILIER_HPP

#include "profiler_administrator.hpp"
#include <chrono>
#include <string>
#include <vector>
#include <thread>

// POUR DÉSACTIVER LES PROFILERS,
// IL SUFFIT D'ACTIVER L'OPTION CMAKE "DISABLE_PROFILER"
// Lire CMakeLists.txt pour voir les options supportées

#ifndef NPROFILER
namespace {
    thread_local std::vector<std::string> parents;
}

template <class T, typename Clock = std::chrono::high_resolution_clock>
class profiler
{
    using time_point = typename Clock::time_point;
    static_assert(is_time<T>::value, "type must be of time");

    time_point begin;
    std::string name;
public:
    explicit profiler(const std::string& name)
    : begin(Clock::now())
    , name(name)
    {
        parents.push_back(name);
    }

    ~profiler()
    {
        time_point end = Clock::now();
        std::string full_name = "/";

        if(!parents.empty()) {
            parents.pop_back();

            for (auto parent : parents) {
                full_name += parent + "/";
            }
            full_name += name;
        }

        profiler_administrator<T>::get_instance().log_time(full_name, begin, end);
    }
};
#else
template <class T, typename Clock = std::chrono::high_resolution_clock>
class profiler
{
    using time_point = typename Clock::time_point;
    static_assert(is_time<T>::value, "type must be of time");

public:
    explicit profiler(const std::string& name) {};
    ~profiler() = default;
};
#endif

using profiler_us = profiler<std::chrono::microseconds>;

#endif