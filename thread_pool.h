#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>
#include <memory>

namespace tmp {
    class ThreadPool {
        private:
            std::mutex mutex;
            uint64_t num_threads;
            std::vector<std::thread> threads;
            std::queue<std::function<void()>> queue;
            std::condition_variable mutex_condition;
            bool m_terminate = false;

            void threadLoop();
            void terminate(); 

        public:
            ThreadPool(int num_threads);
            ThreadPool();
            ~ThreadPool();

            void start();
            void queueJob(const std::function<void()>& job);
            bool isBusy();
            void stop();
    };
}
