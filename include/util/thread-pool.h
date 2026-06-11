#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(unsigned n_threads);
    ~ThreadPool();

    void submit(std::function<void()> task);
    void wait(); // block until all submitted tasks finish

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    void worker_loop();

    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_work_cv;  // wake idle workers
    std::condition_variable m_done_cv;  // wake wait() caller
    unsigned m_in_flight = 0;
    bool m_stop = false;
};
