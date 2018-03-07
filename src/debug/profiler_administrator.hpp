#ifndef DEF_PROFILIER_ADMINISTRATOR_HPP
#define DEF_PROFILIER_ADMINISTRATOR_HPP

#include <memory>
#include <chrono>
#include <fstream>
#include <string>
#include <ctime>
#include <time.h>

template<class T>
struct is_time {
    static const bool value = false;
};

template<>
struct is_time <std::chrono::hours>
{
    static const bool value = true;
};

template<>
struct is_time <std::chrono::minutes>
{
    static const bool value = true;
};

template<>
struct is_time <std::chrono::seconds>
{
    static const bool value = true;
};

template<>
struct is_time <std::chrono::milliseconds>
{
    static const bool value = true;
};

template<>
struct is_time <std::chrono::microseconds>
{
    static const bool value = true;
};

template<>
struct is_time <std::chrono::nanoseconds>
{
    static const bool value = true;
};

template<class T>
class profiler_administrator
{
    static_assert(is_time<T>::value, "the type is not a time");

    profiler_administrator() {
        std::ofstream out_stream("log_time.csv");
        write_header(out_stream);
    }

    void write_header(std::ostream& os) {
        os << "name, time, elapsed" << std::endl;
    }

    void write_row(std::ostream& os, std::string name, std::string time, long duration) {
        os << name << "," << time << "," << duration << std::endl;
    }

    std::string get_current_time() const noexcept {
        const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string time(ctime(&now));
        if(time.back() == '\n') time.pop_back();

        return time;
    }

public:
    static profiler_administrator<T>& get_instance()
    {
        static profiler_administrator<T> instance;
        return instance;
    }

    template<typename TimePoint>
    void log_time(const std::string& name, const TimePoint& begin, const TimePoint& end)
    {
        std::ofstream out_stream("log_time.csv", std::ios_base::app);
        write_row(out_stream, name, get_current_time(), std::chrono::duration_cast<T>(end - begin).count());
    }

};
#endif
