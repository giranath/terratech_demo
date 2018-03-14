#ifndef MMAP_DEMO_THREAD_POOL_HPP
#define MMAP_DEMO_THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
#include <future>

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
    : fn{std::forward<Fn>(fn)} {

    }

    void execute() override {
        fn();
    }
};

template<typename Ret, typename Fn>
class processing_task : public base_task {
    Fn fn;
    std::promise<Ret> promise;
public:
    explicit processing_task(Fn&& fn)
    : fn{std::forward<Fn>(fn)} {

    }

    void execute() override {
        try {
            Ret result = fn();

            promise.set_value(std::move(result));
        }
        catch(...) {
            promise.set_exception(std::current_exception());
        }
    }

    std::future<Ret> get_future() {
        return promise.get_future();
    }
};

template<typename Fn>
std::unique_ptr<task<Fn>> make_task(Fn&& fn) {
    return std::unique_ptr<task<Fn>>(new task(std::forward<Fn>(fn)));
}

template<typename Ret, typename Fn>
std::unique_ptr<processing_task<Ret, Fn>> make_processing_task(Fn&& fn) {
    return std::unique_ptr<processing_task<Ret, Fn>>(new processing_task<Ret, Fn>(std::forward<Fn>(fn)));
}

class thread_pool {
public:
    using task_ptr = std::unique_ptr<base_task>;
private:
    std::vector<std::thread> threads;
    std::queue<task_ptr> tasks;
    std::mutex tasks_mutex;
    std::atomic<bool> is_running;
    std::condition_variable tasks_cv;

    static void worker_thread(thread_pool* pool);
public:
    thread_pool(std::size_t n);
    ~thread_pool() noexcept;

    void push(task_ptr task);

    template<typename It>
    void push(It begin, It end) {
        std::lock_guard<std::mutex> lock(tasks_mutex);

        for(; begin != end; ++begin) {
            tasks.push(std::move(*begin));
            tasks_cv.notify_one();
        }
    }
};


#endif //MMAP_DEMO_THREAD_POOL_HPP
