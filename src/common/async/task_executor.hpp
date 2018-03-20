#ifndef MMAP_DEMO_THREAD_POOL_HPP
#define MMAP_DEMO_THREAD_POOL_HPP

#include "task.hpp"

#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <future>

namespace async {

class task_executor {
public:
    using task_handle = uint32_t;
    using task_ptr = std::unique_ptr<base_task>;
    using task_future = std::future<task_ptr>;
private:
    struct task_value {
        task_ptr value;
        std::promise<task_ptr> promise;

        task_value() noexcept
        : value{} {

        }

        explicit task_value(task_ptr t)
        : value{std::move(t)} {

        }

        task_value(task_value&& other) noexcept
        : value(std::move(other.value))
        , promise(std::move(other.promise)) {

        }

        task_value& operator=(task_value&& other) noexcept {
            value = std::move(other.value);
            promise = std::move(other.promise);

            return *this;
        }
    };

    using queue_element = std::pair<task_handle, task_value>;

    std::atomic<bool> is_running;
    std::vector<std::thread> workers;

    // Store the task waiting to be processed
    std::queue<queue_element> waiting_queue;
    std::mutex waiting_mutex;

    task_handle last_handle;
    std::condition_variable wait_cv;

    static void worker_thread(task_executor* executor);

    queue_element wait_for_next();

public:
    explicit task_executor(std::size_t worker_count);
    ~task_executor();

    task_future push(task_ptr new_task);
};
}

#endif //MMAP_DEMO_THREAD_POOL_HPP
