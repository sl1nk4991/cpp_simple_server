#pragma once

#include "server.h"

namespace tmp {
    class Application {
        private:
            int argc;
            char **argv;

            void start();
        public:
            Application(int argc, char **argv);
    };
}
