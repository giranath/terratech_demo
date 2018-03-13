#ifndef MMAP_DEMO_THREAD_POOL_HPP
#define MMAP_DEMO_THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>

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

class task_executor {
public:
    using task_handle = uint32_t;
    using task_ptr = std::unique_ptr<base_task>;
private:
    struct task_value {
        task_ptr value;
        bool store_when_finished;

        task_value()
        : value{}
        , store_when_finished(false) {

        }

        task_value(task_ptr t, bool b)
        : value{std::move(t)}
        , store_when_finished(b) {

        }

        explicit task_value(task_value&& other)
        : value(std::move(other.value))
        , store_when_finished(other.store_when_finished) {

        }

        task_value& operator=(task_value&& other) {
            value = std::move(other.value);
            store_when_finished = other.store_when_finished;

            return *this;
        }
    };

    using queue_element = std::pair<task_handle, task_value>;

    std::atomic<bool> is_running;
    std::vector<std::thread> workers;

    // Store the task waiting to be processed
    std::queue<queue_element> waiting_queue;
    std::mutex waiting_mutex;

    // Store the processed tasks
    std::vector<queue_element> finished_queue;
    std::mutex finished_mutex;

    task_handle last_handle;
    std::condition_variable wait_cv;
    std::condition_variable finish_cv;

    static void worker_thread(task_executor* executor);

    queue_element wait_for_next();
    void finish_task(queue_element task);

public:
    explicit task_executor(std::size_t worker_count);
    ~task_executor();

    task_handle push(task_ptr new_task, bool store = false);
    task_ptr wait(task_handle handle);
};
}

#endif //MMAP_DEMO_THREAD_POOL_HPP
