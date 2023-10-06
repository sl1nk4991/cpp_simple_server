#include "application.h"
#include "server.h"

namespace tmp {
    Application::Application(int argc, char **argv): argc(argc), argv(argv) {
        this->start();
    }

    void Application::start() {
        Server server(8080);
    }
}