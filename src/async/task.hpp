#ifndef MMAP_DEMO_TASK_HPP
#define MMAP_DEMO_TASK_HPP

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

}

#endif
