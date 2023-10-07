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

    Server::Server(std::string addr, uint16_t port)
        :addr(addr), port(port)
    {
        this->start();
    }
    Server::Server(uint16_t port)
        :port(port)
    {
        this->start();
    }
    Server::Server() {
        this->port = 8080;
        
        this->start();
    }

    Server::~Server() {
        if (this->efd != -1) {
            close(this->efd);
        }
     }

    void Server::start() {
        tmp::sig_obj = this;
        std::signal(SIGINT, signalHandler);

        if (efd != -1) {
            close(this->efd);
        }

        this->efd = eventfd(0, 0);
        if (this->efd == -1) {
            throw std::runtime_error(strerror(errno));
        }

        this->mainFunc();
    }

    void Server::stop() {
        this->m_terminate = true;

        auto ret = write(this->efd, &this->one, sizeof(this->one));
        if (ret == -1) {
            throw std::runtime_error(strerror(errno));
        }
    }

    void Server::mainFunc() {
        std::cout << "listen on port: " << this->port << std::endl;
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if(fd == -1) {
            throw std::runtime_error(strerror(errno));
        }

        if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &this->one, sizeof(this->one)) != 0 ) {
            throw std::runtime_error(strerror(errno));
        }

        struct sockaddr_in server_sock = {0};
        server_sock.sin_family = AF_INET;
        server_sock.sin_port = htons(port);
        if (addr.empty()) {
            server_sock.sin_addr.s_addr = INADDR_ANY;
        } else {
            int ret = inet_aton(addr.c_str(), &server_sock.sin_addr);
            if (!ret) {
                throw std::runtime_error("inet_aton: can't convert ip address");
            }
        }

        if(bind(fd, (sockaddr*)&server_sock, sizeof(server_sock)) != 0) {
            throw std::runtime_error(strerror(errno));
        }

        if(listen(fd, MAX_CON_QUEUE) != 0) {
            throw std::runtime_error(strerror(errno));
        }

        ThreadPool threadPool;            
        std::unordered_map<int, Handler> handlers;
        std::vector<struct pollfd> fds{ { this->efd, POLLIN, 0 }, { fd, POLLIN, 0 } };

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
                    int cfd = accept(fd, NULL, NULL);
                    std::cout << "new connenction" << std::endl;

                    if (cfd != -1) {
                        fds.push_back({cfd, POLLIN, 0});
                        handlers.emplace(cfd, cfd);
                        threadPool.queueJob([&handlers, cfd] {handlers.at(cfd).join();});
                    } else {
                        std::cout << strerror(errno) << std::endl;
                    }

                    fds[1].revents = 0;
                }

                for (auto it = fds.begin() + 2; it != fds.end(); ) {
                    char c;
                    if (it->revents && recv(it->fd, &c, sizeof(c), MSG_PEEK|MSG_DONTWAIT) == 0) {
                        std::cout << "client disconnected" << std::endl;
                        close(it->fd);
                        if (handlers.find(it->fd) != handlers.end())
                        {
                            handlers.at(it->fd).stop();
                            handlers.erase(it->fd);
                        }
                        it = fds.erase(it);
                    } else {
                        it->revents = 0;
                        it++;
                    }
                }
            }
        }

        for (auto it = fds.begin() + 1; it != fds.end(); it++) {
                close(it->fd);
                if (handlers.find(it->fd) != handlers.end()) {
                    handlers.at(it->fd).stop();
                    handlers.erase(it->fd);
                }
        }

        threadPool.stop();
        std::cout << "server stoped" << std::endl;
    }
}

