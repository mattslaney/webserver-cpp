#include <cstddef>
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <utility>

#include "ThreadPool.h"

namespace sws {
  ThreadPool::ThreadPool(size_t numThreads) {
    for (size_t i = 0; i < numThreads; i++) {
      m_threads.emplace_back([this, i] {
        while (true) {
          std::function<void(size_t)> task;

          // Retreive any tasks from the queue or exit if stopped
          {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_cv.wait(lock, [this] {
              return !m_tasks.empty() || m_stop;
            });
            if(m_stop && m_tasks.empty()) {
              return;
            }
            task = std::move(m_tasks.front());
            m_tasks.pop();
          }

          task(i);
        }
      });
    }
  }

  ThreadPool::~ThreadPool() {
    {
      std::unique_lock<std::mutex> lock(m_queueMutex);
      m_stop = true;
    }

    m_cv.notify_all();

    for (auto& thread : m_threads) {
      thread.join();
    }
  }

  void ThreadPool::enqueue(std::function<void (size_t threadNum)> task) {
    {
      std::unique_lock<std::mutex> lock(m_queueMutex);
      m_tasks.emplace(move(task));
    }
    m_cv.notify_one();
  }
}

