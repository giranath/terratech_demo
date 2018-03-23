#include "task_executor.hpp"

#include <algorithm>
#include <iterator>

namespace async {

void task_executor::worker_thread(task_executor* executor) {
    while(executor->is_running) {
        auto next = executor->wait_for_next();

        if(next.second.value) {
            try {
                next.second.value->execute();
                next.second.promise.set_value(std::move(next.second.value));
            }
            catch(...) {
                next.second.promise.set_exception(std::current_exception());
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

task_executor::task_executor(memory::heap_allocator& allocator, std::size_t worker_count)
: is_running(true)
, workers(memory::container_heap_allocator<std::thread>(allocator))
, waiting_queue(memory::container_heap_allocator<queue_element>(allocator))
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

task_executor::task_future task_executor::push(task_ptr new_task) {
    task_handle handle = last_handle + 1;
    std::future<task_ptr> future;
    {
        ++last_handle;

        std::lock_guard<std::mutex> lock(waiting_mutex);

        task_value val(std::move(new_task));
        future = val.promise.get_future();
        waiting_queue.emplace(handle, std::move(val));
    }
    wait_cv.notify_one();

    return future;
}

}