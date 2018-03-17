#ifndef MMAP_DEMO_TASK_HPP
#define MMAP_DEMO_TASK_HPP

#include <memory>

namespace async {

class base_task {
public:
    virtual ~base_task() = default;
    virtual void execute() = 0;
};

template<typename Fn>
class task : public base_task {
    Fn fn;
public:
    explicit task(Fn&& fn)
    : fn{fn} {

    }

    void execute() override {
        fn();
    }
};

template<typename FN>
std::unique_ptr<task<FN>> make_task(FN&& fn) {
	return std::make_unique<task<FN>>(std::forward<FN>(fn));
}

}

#endif
