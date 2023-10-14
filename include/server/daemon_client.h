#pragma once

#include <string>

namespace tmp {
    class DaemonClient {
        private:
            std::string msg;

            void start();
        public:
            DaemonClient(char*);
    };
}