#include "resources/thread_pool.h"
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace Resources {
    ThreadPool::ThreadPool(int n): num_threads(n) {
        this->start();
    }

    ThreadPool::ThreadPool() {
        //this->num_threads = std::thread::hardware_concurrency();
        this->num_threads = 3;

        this->start();
    }

    ThreadPool::~ThreadPool() {
        this->terminate();
    }

    void ThreadPool::start() {
        for(int i = 0; i < this->num_threads; i++) {
            threads.emplace_back(std::thread([this]() { this->threadLoop(); }));
            pthread_setname_np(threads.back().native_handle(), std::string("Worker " + std::to_string(i)).c_str());
        }
    }

    void ThreadPool::queueJob(const std::function<void()>& job) {
        {
            std::unique_lock<std::mutex> lock(this->mutex);
            this->queue.push(job);
        }
        mutex_condition.notify_one();
    }

    void ThreadPool::stop() {
        this->m_terminate = true;
        mutex_condition.notify_all();

        for(auto& active_thread: threads) {
            active_thread.join();
        }

        threads.clear();
    }

    void ThreadPool::terminate() {
        for(auto& active_thread: threads) {
            active_thread.~thread();
        }

        threads.clear();
    }

    void ThreadPool::threadLoop() {
        while (true) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(this->mutex);
                this->mutex_condition.wait(
                    lock, [this] { return !this->queue.empty()||this->m_terminate; }
                );
                if(this->m_terminate)
                    return;
                job = this->queue.front();
                this->queue.pop();
            }
            job();
        }
    }
}

