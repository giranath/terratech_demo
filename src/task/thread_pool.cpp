#include "thread_pool.hpp"

void thread_pool::worker_thread(thread_pool* pool) {
    while(pool->is_running) {
        std::unique_lock<std::mutex> lock(pool->tasks_mutex);

        if(pool->tasks.empty()) {
            pool->tasks_cv.wait(lock, [=]() { return !pool->is_running || !pool->tasks.empty(); });

            if(!pool->is_running) {
                return;
            }
        }

        task_ptr front_task = std::move(pool->tasks.front());
        pool->tasks.pop();
        lock.unlock();

        // Now we can execute the task
        front_task->execute();
    }
}

thread_pool::thread_pool(std::size_t n)
: is_running{true} {
    for(std::size_t i = 0; i < n; ++i) {
        threads.emplace_back(worker_thread, this);
    }
}

thread_pool::~thread_pool() noexcept {
    is_running = false;
    tasks_cv.notify_all();

    // Joins every threads
    for(std::thread& t : threads) {
        t.join();
    }
}

void thread_pool::push(task_ptr task) {
    std::lock_guard<std::mutex> lock{tasks_mutex};
    tasks.push(std::move(task));
    tasks_cv.notify_one();
}