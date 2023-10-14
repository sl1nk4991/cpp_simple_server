#include "tcp_socket.h"
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <system_error>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <utility>

#define CHECK(x) if ((x) == -1) {throw std::system_error(errno, std::system_category(), strerror(errno));}

namespace Resources {
    TCPSocket::TCPSocket(const uint32_t fd): fd(-1) {
        if (fd) {
            this->fd = fd;
        } else {
            this->fd = socket(AF_INET, SOCK_STREAM, 0);
            CHECK(this->fd)
        }
    }

    /*
     * TODO
     * Write move semantics
     *TCPSocket::TCPSocket(TCPSocket&& move) noexcept:
     *    TCPSocket(std::exchange(move.fd, -1)) {};

     *TCPSocket& TCPSocket::operator=(TCPSocket&& move) noexcept {
     *    move.fd = -1;
     *    return *this;
     *}
     */

    TCPSocket::~TCPSocket() {
        if (this->fd != -1) {
            close(this->fd);
        }
    }

    void TCPSocket::Bind(uint16_t port, std::string addr) {
        {
            int one = 1;
            setsockopt(this->fd, SOCK_STREAM, SO_REUSEADDR, &one, sizeof(one));
        }

        struct sockaddr_in sock;
        memset(&sock, 0, sizeof(sock));
        sock.sin_family = AF_INET;
        sock.sin_port = htons(port);
        if (!addr.empty()) {
            int ret = inet_aton(addr.c_str(), &sock.sin_addr);
            if (ret == 0) {
                throw std::runtime_error("inet_aton: can't convert ip address");
            }
        } else {
            sock.sin_addr.s_addr = INADDR_ANY;
        }

        int ret = bind(this->fd, (sockaddr*)&sock, sizeof(sock));
        CHECK(ret)
    }

    void TCPSocket::Connect(uint16_t port, std::string addr) {
        struct sockaddr_in sock;
        memset(&sock, 0, sizeof(sock));
        sock.sin_family = AF_INET;
        sock.sin_port = htons(port);
        if (!addr.empty()) {
            int ret = inet_aton(addr.data(), &sock.sin_addr);
            if (ret == 0) {
                throw std::runtime_error("inet_aton: can't convert ip address");
            }
        } else {
            throw std::invalid_argument("IP address can't be empty");
        }

        int ret = connect(this->fd, (sockaddr*)&sock, sizeof(sock));
        CHECK(ret)
    }

    void TCPSocket::Listen(uint32_t max_con_queue) {
        if (max_con_queue == 0) {
            throw std::invalid_argument("Maximum connection queue can't be zero");
        }

        int ret = listen(this->fd, max_con_queue);
        CHECK(ret)
    }

    //TODO
    //Allow receive client address
    //In methods: Accept, Recv, Send
    uint32_t TCPSocket::Accept() {
        int ret = accept(this->fd, NULL, NULL);
        CHECK(ret)

        return ret;
    }

    uint64_t TCPSocket::Recv(std::string& str) {
        if (str.size() == 0) {
            throw std::invalid_argument("Buffer can't be zero size");
        }

        int ret = recv(this->fd, str.data(), str.size(), 0);
        CHECK(ret)

        return ret;
    }

    uint64_t TCPSocket::Send(const std::string& str) {
        if (str.size() == 0) {
            throw std::invalid_argument("Buffer can't be zero size");
        }

        int ret = send(this->fd, str.data(), str.size(), 0);
        CHECK(ret)

        return ret;
    }

    void TCPSocket::Shutdown(SHUT_METHOD method) {
        int ret = shutdown(this->fd, method);
        CHECK(ret)
    }

    int TCPSocket::getFd() {
        return this->fd;
    }
}
