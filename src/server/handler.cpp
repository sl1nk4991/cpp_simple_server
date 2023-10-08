#include "handler.h"
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <pthread.h>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <cstring>
#include <vector>
#include <sys/eventfd.h>
#include <unistd.h>

namespace tmp {
    Handler::Handler(int fd): sock(fd) {}

    Handler::~Handler() {
        this->stop();
    }

    void Handler::stop() {
        this->m_terminate = true;

        event.Write();
    }

    void Handler::start() {
        std::string buff(BUFFSIZE, '\0');
        std::vector<struct pollfd> fds = { {this->event.getFd(), POLLIN, 0}, {this->sock.getFd(), POLLIN, 0} };

        try {
            while (!this->m_terminate) {
                int n = poll(fds.data(), fds.size(), -1);
                if (n == -1) {
                    throw std::runtime_error(strerror(errno));
                }

                if (n > 0) {
                    if (fds[0].revents) {
                        std::cout << "handle thread terminated" << std::endl;
                        break;
                    } else if (fds[1].revents) {
                        char c;
                        if (recv(this->event.getFd(), &c, sizeof(c), MSG_PEEK|MSG_DONTWAIT) == 0) {
                            std::cout << "client disconnected" << std::endl;
                            break;
                        }
                        sock.Recv(buff);
                        if (buff.data()[0] != '\0') {
                            std::cout << "Messsage: " << buff << std::endl;
                            sock.Send(buff);
                        }
                        std::memset(buff.data(), 0, buff.size());
                    }
                }
            }
        } catch(const std::system_error& e) {
            if (e.code().value() == ECONNRESET) {
                std::cout << "handler: " << e.code().message() << std::endl;
                return;
            }
        }

        sock.Shutdown();
    }
}
