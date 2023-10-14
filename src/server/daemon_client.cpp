#include "daemon_client.h"
#include "unix_socket.h"

#include <iostream>

namespace tmp {
    DaemonClient::DaemonClient(char* msg): msg(msg) {
        this->start();        
    } 

    void DaemonClient::start() {
        UNIXSocket server;

        try {
            std::string path = SOCK_NAME;
            server.Connect(path);
        } catch (const std::system_error& e) {
            if (e.code().value() == ECONNREFUSED) {
                std::cerr << e.code().message() << std::endl;
                exit(EXIT_FAILURE);
            }

            throw e;
        }

        server.Send(msg);
        msg = server.Recv();

        std::cout << msg << std::endl;
    }
}
