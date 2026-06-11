#include "util/thread-pool.h"

ThreadPool::ThreadPool(unsigned n_threads)
{
    m_workers.reserve(n_threads);
    for (unsigned i = 0; i < n_threads; ++i)
        m_workers.emplace_back(&ThreadPool::worker_loop, this);
}

ThreadPool::~ThreadPool()
{
    {
        std::lock_guard lock(m_mutex);
        m_stop = true;
    }
    m_work_cv.notify_all();
    for (auto& t : m_workers)
        t.join();
}

void ThreadPool::submit(std::function<void()> task)
{
    {
        std::lock_guard lock(m_mutex);
        m_queue.push(std::move(task));
        ++m_in_flight;
    }
    m_work_cv.notify_one();
}

void ThreadPool::wait()
{
    std::unique_lock lock(m_mutex);
    m_done_cv.wait(lock, [this] { return m_in_flight == 0; });
}

void ThreadPool::worker_loop()
{
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock lock(m_mutex);
            m_work_cv.wait(lock, [this] { return m_stop || !m_queue.empty(); });
            if (m_stop && m_queue.empty())
                return;
            task = std::move(m_queue.front());
            m_queue.pop();
        }

        task();

        {
            std::lock_guard lock(m_mutex);
            if (--m_in_flight == 0)
                m_done_cv.notify_one();
        }
    }
}
