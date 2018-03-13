#include "thread_pool.hpp"

#include <algorithm>
#include <iterator>

namespace async {

void task_executor::worker_thread(task_executor* executor) {
    while(executor->is_running) {
        auto next = executor->wait_for_next();

        if(next.second.value) {
            next.second.value->execute();

            if(next.second.store_when_finished) {
                executor->finish_task(std::move(next));
            }
        }
    }
}

task_executor::queue_element task_executor::wait_for_next() {
    std::unique_lock<std::mutex> lock(waiting_mutex);

    wait_cv.wait(lock, [this]() { return !waiting_queue.empty() || !is_running; });

    if(is_running) {
        auto next_task = std::move(waiting_queue.front());
        waiting_queue.pop();

        return next_task;
    }

    return queue_element(0, task_value{});
}

void task_executor::finish_task(queue_element task) {
    {
        std::lock_guard<std::mutex> lock(finished_mutex);

        // finished_queue is always sorted
        auto it = std::lower_bound(std::begin(finished_queue), std::end(finished_queue), task,
                                   [](const queue_element &a, const queue_element &b) { return a.first < b.first; });
        finished_queue.insert(it, std::move(task));
    }
    finish_cv.notify_one();
}

task_executor::task_executor(std::size_t worker_count)
: is_running(true)
, last_handle(0) {
    for(std::size_t i = 0; i < worker_count; ++i) {
        workers.emplace_back(worker_thread, this);
    }
}

task_executor::~task_executor() {
    is_running = false;
    wait_cv.notify_all();

    for(std::thread& t : workers) {
        t.join();
    }
}

task_executor::task_handle task_executor::push(task_ptr new_task, bool store) {
    task_handle handle = last_handle + 1;
    {
        ++last_handle;

        std::lock_guard<std::mutex> lock(waiting_mutex);
        waiting_queue.emplace(handle, task_value(std::move(new_task), store));
    }
    wait_cv.notify_one();

    return handle;
}

task_executor::task_ptr task_executor::wait(task_handle handle) {
    std::unique_lock<std::mutex> lock(finished_mutex);
    finish_cv.wait(lock, [this, handle]() {
        return std::binary_search(std::begin(finished_queue),
                                  std::end(finished_queue),
                                  queue_element(handle, task_value{}),
                                  [](const queue_element& a, const queue_element& b) { return a.first < b.first; });
    });

    auto it = std::lower_bound(std::begin(finished_queue),
                               std::end(finished_queue),
                               queue_element(handle, task_value{}),
                               [](const queue_element& a, const queue_element& b) { return a.first < b.first; });

    if(it != std::end(finished_queue)) {
        task_ptr task = std::move(it->second.value);
        finished_queue.erase(it);

        return task;
    }

    return nullptr;
}

}