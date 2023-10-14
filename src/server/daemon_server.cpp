#include "daemon_server.h"
#include "unix_socket.h"
#include "server.h"

#include <iostream>
#include <sys/poll.h>

namespace tmp {
    DaemonServer::DaemonServer() {
        this->start();
    }

    void DaemonServer::start() {
        UNIXSocket dserver; 

        try {
            std::string path = SOCK_NAME;
            dserver.Bind(path);
        } catch (const std::system_error& e) {
            if (e.code().value() == EADDRINUSE) {
                std::cerr << e.code().message() << std::endl;
                exit(EXIT_FAILURE);
            }

            throw e;
        }

        std::cout << "Server running as daemon" << std::endl;

        daemon(0, 0);

        Server server;
        std::thread thread([&server] { server.start(); });

        dserver.Listen(10);

        std::string ret, msg;
        std::vector<std::unique_ptr<UNIXSocket>> clients;
        std::vector<struct pollfd> fds { { dserver.getFd(), POLLIN, 0} };

        while (!this->m_terminate) {
            int n = poll(fds.data(), fds.size(), 1);
            SYSCHECK(n)

            if (fds[0].revents & POLLIN) {
                clients.push_back(std::make_unique<UNIXSocket>(dserver.Accept()));
                fds.push_back({clients.back()->getFd(), POLLIN, 0});
            } else if (fds.size() > 1) {
                for (auto it = fds.begin() + 1; it != fds.end();) {
                    const auto& client = std::find_if(clients.begin(), clients.end(),
                        [&it] (auto& ptr){
                            return ptr->getFd() == it->fd;
                        }
                    );

                    if (it->revents & POLLHUP) {
                        clients.erase(client);
                        it = fds.erase(it);
                        continue;
                    } else if (it->revents & POLLIN) {
                        msg = client->get()->Recv();

                        if (msg == "STOP") {
                            server.stop();

                            ret = "Server stopped";
                            client->get()->Send(ret);

                            this->m_terminate = true;
                            continue;
                        }

                        ret = "Server: " + msg;
                        client->get()->Send(ret);

                        ret.clear();
                    }

                    it++;
                }
            }
        }
    }
}