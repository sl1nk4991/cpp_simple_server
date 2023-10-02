#pragma once

#include <string>
#include <thread>
#include "thread_pool.h"
#include "handler.h"

#define TIMEOUT         1000
#define MAX_CONS        100
#define MAX_BUFF_LENGTH 4096

namespace tmp {
    class Server {
        private:
            int efd = -1;
            uint64_t one = 1;
            uint16_t    port;
            std::string addr;
            std::thread m_thread;
            bool m_terminate = false;

            void start();
            void threadFunc();

        public:
            Server(std::string addr, uint16_t port);
            ~Server();

            void join();
            void terminate();
    };

    extern "C" void signalHandler(int sig);
    static Server *sig_obj;
}

