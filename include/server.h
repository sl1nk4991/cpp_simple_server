#pragma once

#include "resources/thread_pool.h"
#include "resources/socket.h"
#include "resources/event.h"
#include "handler.h"

#include <string>

#define MAX_CON_QUEUE   10
#define MAX_BUFF_LENGTH 4096

namespace tmp {
    class Server {
        private:
            Resources::Event event;
            bool m_terminate = false;

            std::string addr;
            uint16_t    port;

            void start();
            void mainFunc();

        public:
            Server(uint16_t port = 8080, std::string addr = "");
            Server();
            ~Server();

            void stop();
    };

    extern "C" void signalHandler(int sig);
    static Server *sig_obj;
}

