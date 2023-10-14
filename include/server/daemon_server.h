#pragma once

namespace tmp {
    class DaemonServer {
        private:
            bool m_terminate = false;

            void start();
        public:
            DaemonServer();
    };
}
