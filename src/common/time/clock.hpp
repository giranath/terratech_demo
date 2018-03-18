#ifndef MMAP_DEMO_TIME_HPP
#define MMAP_DEMO_TIME_HPP

#include <chrono>

namespace game_time {

template<typename concrete_clock>
class clock {
    using frame_duration = typename concrete_clock::duration;
    using time_point = typename concrete_clock::time_point;

    time_point start;

public:
    clock() noexcept : start{concrete_clock::now()} {

    }

    void restart() noexcept {
        start = concrete_clock::now();
    }

    template<typename duration>
    duration elapsed_time() const noexcept {
        return std::chrono::duration_cast<duration>(concrete_clock::now() - start);
    }

    template<typename duration>
    void substract(duration d) {
        start += d;
    }

};

using highres_clock = clock<std::chrono::high_resolution_clock>;

}


#endif //MMAP_DEMO_TIME_HPP
