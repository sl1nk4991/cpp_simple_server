#include "server.h"

#include <cerrno>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <cassert>
#include <unistd.h>
#include <sys/eventfd.h>
#include <vector>
#include <poll.h>
#include <unordered_map>

namespace tmp {
    void signalHandler(int sig) {
        sig_obj->stop();
    }

    Server::Server(uint16_t port, std::string addr)
        :addr(addr), port(port) {}

    Server::~Server() {}

    void Server::stop() {
        this->m_terminate = true;

        event.Write();
    }

    void Server::start() {
        tmp::sig_obj = this;
        std::signal(SIGINT, signalHandler);

        std::cout << "listen on port: " << this->port << std::endl;
        Resources::TCPSocket sock;
        sock.Bind(this->port, this->addr);
        sock.Listen(MAX_CON_QUEUE);

        std::vector<std::unique_ptr<Handler>> handlers;
        Resources::ThreadPool threadPool;
        std::vector<struct pollfd> fds{ { this->event.getFd(), POLLIN, 0 }, { sock.getFd(), POLLIN, 0 } };

        while (!this->m_terminate) {
            int n = poll(fds.data(), fds.size(), -1);
            if (n == -1) {
                if (errno != EINTR) {
                    throw std::runtime_error(strerror(errno));
                }
            }

            if (n > 0) {
                if (fds[0].revents) {
                    std::cout << "stop request recived" << std::endl;
                    break;
                } else if(fds[1].revents) {
                    int cfd = sock.Accept();
                    std::cout << "new connenction" << std::endl;

                    if (cfd != -1) {
                        handlers.emplace_back(std::make_unique<Handler>(cfd));
                        threadPool.queueJob([&handlers] {handlers.back().get()->start();});
                    } else {
                        std::cout << strerror(errno) << std::endl;
                    }

                    fds[1].revents = 0;
                }
            }
        }

        for (auto it = handlers.begin(); it != handlers.end(); it++) {
            it.base()->get()->stop();
        }

        threadPool.stop();
        std::cout << "server stoped" << std::endl;
    }
}
