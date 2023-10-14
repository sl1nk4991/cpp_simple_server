#include "unix_socket.h"

namespace tmp {
    UNIXSocket::UNIXSocket(int fd): buff(BUFF_SIZE) {
        if (fd == -1) {
            this->initialize();
        } else {
            this->fd = fd;
        }
    }

    UNIXSocket::~UNIXSocket() {
        if (this->fd != -1) {
            close(this->fd);
        }
    }

    void UNIXSocket::initialize() {
        this->fd = socket(AF_UNIX, SOCK_STREAM, 0);
        SYSCHECK(this->fd)
    }

    int& UNIXSocket::getFd() { return this->fd; }

    void UNIXSocket::Bind(std::string& path) {
        if (path.size() > 108 || path.size() == 0) {
            throw std::invalid_argument("invalid path");
        }

        struct sockaddr_un addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, path.data(), path.size());
        addr.sun_path[0] = 0;

        {
            int ret = bind(
                this->fd,
                (sockaddr*)&addr,
                sizeof(sa_family_t) + path.size()
            );
            SYSCHECK(ret)
        }
    }

    void UNIXSocket::Connect(std::string& path) {
        if (path.size() > 108 || path.size() == 0) {
            throw std::invalid_argument("invalid path");
        }

        struct sockaddr_un addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, path.data(), path.size());
        addr.sun_path[0] = 0;

        {
            int ret = connect(
                this->fd,
                (sockaddr*)&addr,
                sizeof(sa_family_t) + path.size()
            );
            SYSCHECK(ret)
        }
    }

    void UNIXSocket::Listen(int backlog) {
        int ret = listen(this->fd, backlog);
        SYSCHECK(ret)
    }

    int UNIXSocket::Accept() {
        int ret = accept(this->fd, NULL, NULL);
        SYSCHECK(ret)

        return ret;
    }

    std::string& UNIXSocket::Recv() {
        {
            int ret = recv(this->fd, this->buff.data(), this->buff.size(), 0);
            SYSCHECK(ret)
        }
        this->buff[this->buff.size() - 1] = 0;

        this->msg.clear();

        this->msg.insert(0, this->buff.data());

        std::fill(buff.begin(), buff.end(), 0);

        return this->msg;
    }

    void UNIXSocket::Send(std::string& msg) {
        if (msg.size() != 0) {
            int ret = send(this->fd, msg.data(), msg.size(), 0);
            SYSCHECK(ret)
        }
    }

    void UNIXSocket::Shutdown() {
        int ret = shutdown(this->fd, SHUT_RDWR);
        SYSCHECK(ret)
    }
}
