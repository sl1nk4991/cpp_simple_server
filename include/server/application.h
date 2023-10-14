#pragma once

#include "server.h"

namespace tmp {
    class Application {
        private:
            int argc;
            char **argv;

            void start();
            void printHelp();
            void printInvArg();

        public:
            Application(int argc, char **argv);
    };
}

