#ifndef MMAP_DEMO_EVENT_HPP
#define MMAP_DEMO_EVENT_HPP

#include <functional>
#include <vector>

namespace async {

template<typename... Args>
class event {
public:
    using handler = std::function<void(Args...)>;
private:
    std::vector<handler> handlers;

public:
    template<typename HANDLER_FN>
    void attach(HANDLER_FN&& fn) {
        handlers.push_back(std::forward<HANDLER_FN>(fn));
    }

    void call(Args... arguments) {
        std::for_each(std::begin(handlers), std::end(handlers), [&](const handler& handler) {
           handler(arguments...);
        });
    }
};

}

#endif //MMAP_DEMO_EVENT_HPP
