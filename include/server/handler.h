#pragma once

#include "event.h"
#include "tcp_socket.h"

#include <cstdint>
#include <string>
#include <memory>

#define BUFFSIZE 1024

namespace tmp {
    class Handler {
        private:
            uint64_t one = 1;
            Resources::Event event;
            bool m_terminate = false;
            Resources::TCPSocket sock;

        public:
            explicit Handler(int fd);
            ~Handler();

            void start();
            void stop();
    };
}
