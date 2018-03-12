#ifndef MMAP_DEMO_THREAD_POOL_HPP
#define MMAP_DEMO_THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>

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

    virtual void execute() {
        fn();
    }
};

template<typename Fn>
std::unique_ptr<base_task> make_task(Fn&& fn) {
    return std::unique_ptr<base_task>(new task(std::forward<Fn>(fn)));
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
