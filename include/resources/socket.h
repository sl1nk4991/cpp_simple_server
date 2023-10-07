#pragma once

#include <cstdint>
#include <string>
#include <sys/socket.h>

namespace Resources {
    class TCPSocket {
        protected:
            int fd;

        public:
            enum SHUT_METHOD { RDWR = SHUT_RDWR, RD = SHUT_RD, WR = SHUT_WR};

        public:
            explicit TCPSocket(const uint32_t = 0);
            TCPSocket(const TCPSocket&) = delete;
            TCPSocket(TCPSocket&&) = delete;
            TCPSocket& operator=(const TCPSocket&) = delete;
            TCPSocket& operator=(TCPSocket&&) = delete;
            ~TCPSocket();

            void Bind(uint16_t, std::string = "");
            void Connect(uint16_t, std::string);
            void Listen(uint32_t = 10);
            uint32_t Accept();
            uint64_t Recv(std::string&);
            uint64_t Send(const std::string&);
            void Shutdown(SHUT_METHOD = RDWR);
            int getFd();
    };
}

