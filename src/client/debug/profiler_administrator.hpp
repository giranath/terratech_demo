#ifndef DEF_PROFILIER_ADMINISTRATOR_HPP
#define DEF_PROFILIER_ADMINISTRATOR_HPP

#include <memory>
#include <chrono>
#include <fstream>
#include <string>
#include <ctime>
#include <time.h>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

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

#ifndef NPROFILER
    struct log_record {
        std::string name;
        std::string time;
        long duration;

    public:
        log_record(const std::string& name, std::string time, long duration)
        : name(name)
        , time(std::move(time))
        , duration(duration) {

        }
    };

    std::vector<log_record> records;
    std::mutex records_mutex;
    std::thread background_thread;
    std::atomic<bool> is_running;

    static void background_thread_function(profiler_administrator* admin) {
        std::ofstream out_stream("log_time.csv");
        admin->write_header(out_stream);

        while(admin->is_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            // Swap records from admin
            std::vector<log_record> temp_records;
            {
                std::lock_guard<std::mutex> lock(admin->records_mutex);
                temp_records.swap(admin->records);
            }

            for(const log_record& record : temp_records) {
                admin->write_row(out_stream, record.name, record.time, record.duration);
            }
        }
    }

    profiler_administrator()
    : background_thread(background_thread_function, this)
    , is_running{true} {

    }

    ~profiler_administrator() {
        is_running = false;
        background_thread.join();
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
#endif
public:
    static profiler_administrator<T>& get_instance()
    {
        static profiler_administrator<T> instance;
        return instance;
    }

    template<typename TimePoint>
    void log_time(const std::string& name, const TimePoint& begin, const TimePoint& end)
    {
#ifndef NPROFILER
        std::lock_guard<std::mutex> lock(records_mutex);
        records.emplace_back(name, get_current_time(), std::chrono::duration_cast<T>(end - begin).count());
#endif
    }

};
#endif