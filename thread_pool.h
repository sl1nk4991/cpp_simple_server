#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>
#include <memory>
#include "handler.h"

namespace tmp {
    class ThreadPool {
        private:
            std::mutex mutex;
            std::queue<int> queue;
            std::vector<std::thread> threads;
            std::condition_variable mutex_condition;
            std::unordered_map<int, Handler> handlers;
            bool m_terminate = false;

            void threadLoop();

        public:
            ThreadPool();
            ~ThreadPool();

            void start();
            void queueJob(int fd);
            std::shared_ptr<std::unordered_map<int, Handler>> getHandlers();
            void terminate();
    };
}
