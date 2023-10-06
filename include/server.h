#pragma once

#include <string>
#include "thread_pool.h"
#include "handler.h"

#define MAX_CON_QUEUE   10
#define MAX_BUFF_LENGTH 4096

namespace tmp {
    class Server {
        private:
            int         efd = -1;
            uint64_t    one = 1;
            bool        m_terminate = false;

            uint16_t    port;
            std::string addr;

            void start();
            void mainFunc();

        public:
            Server(std::string addr, uint16_t port);
            Server(uint16_t port);
            Server();
            ~Server();

            void stop();
    };

    extern "C" void signalHandler(int sig);
    static Server *sig_obj;
}

