#include "application.h"
#include "server.h"
#include "daemon_server.h"
#include "daemon_client.h"

#include <iostream>
#include <cstring>

namespace tmp {
    Application::Application(int argc, char **argv): argc(argc), argv(argv) {
        this->start();
    }

    void Application::start() {
        if (argc == 2 && (!strcmp(argv[1], "-d") || !strcmp(argv[1], "--daemonize"))) {
            DaemonServer(); 
        } else if (argc == 3 && (!strcmp(argv[1], "-s") || !strcmp(argv[1], "--send"))) {
            DaemonClient(this->argv[2]); 
        } else if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
            this->printHelp(); 
        } else if (argc > 1) {
            this->printInvArg();
        } else {
            Server().start();
        }
    }

    void Application::printHelp() {
        std::cout << "Usage " << argv[0] << " [OPTION]...\n"
                << "  -h, --help        display this help and exit\n"
                << "  -d, --daemonize   daemonize application\n"
                << "  -s, --send MSG    send message to daemon" << std::endl;

        exit(EXIT_SUCCESS);
    }

    void Application::printInvArg() {
        std::cerr << argv[0] << ": unrecognized option: " << argv[1] << '\n'
            << "Try: '" << argv[0] << " --help' for more information" << std::endl;

        exit(EXIT_FAILURE);
    }
}
