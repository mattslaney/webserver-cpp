#ifndef thread_pool_h
#define thread_pool_h

#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace sws {
  class ThreadPool {
    public:
      ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
      ~ThreadPool();
      void enqueue(std::function<void(size_t)> task);

    private:
      std::vector<std::thread> m_threads{};
      std::queue<std::function<void(size_t)>> m_tasks{};
      std::mutex m_queueMutex{};
      std::condition_variable m_cv{};
      bool m_stop = false;

  };
}

#endif
