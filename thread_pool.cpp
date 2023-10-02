#include "thread_pool.h"
#include "handler.h"
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace tmp {
    ThreadPool::ThreadPool() {
        this->start();
    }

    ThreadPool::~ThreadPool() {
        this->terminate();
    }

    void ThreadPool::start() {
        int n = std::thread::hardware_concurrency();

        for(int i = 0; i < n; i++) {
            std::cout << "thread " << i << " created" << std::endl;
            threads.emplace_back(std::thread([this]() { this->threadLoop(); }));
            pthread_setname_np(threads.back().native_handle(), std::string("Worker " + std::to_string(i)).c_str());
        }
    }

    void ThreadPool::queueJob(int fd) {
        {
            std::unique_lock<std::mutex> lock(this->mutex);
            this->queue.push(fd);
        }
        mutex_condition.notify_one();
    }

    std::shared_ptr<std::unordered_map<int, Handler>> ThreadPool::getHandlers() {
        return std::make_shared<std::unordered_map<int, Handler>>(this->handlers);
    }

    void ThreadPool::terminate() {
        {
            //std::unique_lock<std::mutex> lock(this->mutex);
            this->m_terminate = true;
        }
        mutex_condition.notify_all();

        for (auto& [fd, handler]: this->handlers) {
            if (handlers.find(fd) != this->handlers.end()) {
                handler.terminate();
            }

        }

        for(auto& active_thread: threads) {
            active_thread.join();
        }

        threads.clear();
    }

    void ThreadPool::threadLoop() {
        while (!this->m_terminate) {
            int fd;
            {
                std::unique_lock<std::mutex> lock(this->mutex);
                this->mutex_condition.wait(
                    lock, [this] { return !this->queue.empty()||this->m_terminate; }
                );
                if(this->m_terminate)
                    return;
                fd = this->queue.front();
                this->queue.pop();
                this->handlers.emplace(fd, fd);
            }
        }
    }
}

