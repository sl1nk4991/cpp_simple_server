#pragma once

#include <cstdint>
#include <string>
#include <memory>

#define BUFFSIZE 1024

namespace tmp {
    class Handler {
        private:
            int fd, efd = -1;
            uint64_t one = 1;
            bool m_terminate = false;

            std::unique_ptr<std::string> recvMessage();
            void sendMessage(const std::string&);

            void start();
            void mainFunc();

        public:
            explicit Handler(int fd);
            ~Handler();

            void join();
            void stop();
            bool isFdExsit(int fd);
    };
}
