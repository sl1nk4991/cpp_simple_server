#pragma once

#include <string>
#include <vector>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <execution>
#include <cstring>

#define SYSCHECK(X) if ((X) == -1) { throw std::system_error(errno, std::system_category(), strerror(errno)); }

#define SOCK_NAME "@cpp_simple_server.sock"
#define BUFF_SIZE 1024

namespace tmp {
    class UNIXSocket {
        private:
            int fd;
            std::string msg;
            std::vector<char> buff;

            void initialize();

        public:
            UNIXSocket(UNIXSocket&&) = delete;
            UNIXSocket(const UNIXSocket&) = delete;
            UNIXSocket& operator=(UNIXSocket&&) = default;
            UNIXSocket& operator=(const UNIXSocket&) = delete;

            UNIXSocket(int fd = -1);
            ~UNIXSocket();

            int& getFd();
            void Bind(std::string& path);
            void Connect(std::string& path);
            void Listen(int backlog);
            int Accept();
            std::string& Recv();
            void Send(std::string& msg);
            void Shutdown();
    };
}
