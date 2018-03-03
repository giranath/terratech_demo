#ifndef DEF_PROFILIER_ADMINISTRATOR_HPP
#define DEF_PROFILIER_ADMINISTRATOR_HPP

#include <memory>
#include <chrono>
#include <fstream>
#include <string>
#include <ctime>

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

    profiler_administrator() {}
public:
    static profiler_administrator<T>& get_instance()
    {

        static profiler_administrator<T> instance;
        return instance;
    }

    void log_time(const std::string& name, const std::chrono::steady_clock::time_point& begin, const std::chrono::steady_clock::time_point& end)
    {
        auto elapsed = std::chrono::duration_cast<T>(end - begin).count();

        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::ofstream out_stream;

        out_stream.open("log_time.csv", std::ios_base::app);
        std::string time;
        time.resize(26);
        ctime_s(&(time[0]), time.size(), &now);
        for (auto it = time.rbegin(); it != time.rend(); ++it)
        {
            if (time.back() == '\n' || time.back() == '\0')
            {
                time.pop_back();
            }
            else
            {
                break;
            }
        }
        out_stream << name << "," << time << "," << elapsed << "," << std::endl;
    }

};
#endif
