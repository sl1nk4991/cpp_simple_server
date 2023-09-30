#pragma once

#include <iostream>
#include <thread>

#define MAX_CONS 100
#define MAX_BUFF_LENGTH 4096
#define TIMEOUT 1000

namespace tmp {
	class Server {
		private:
			int			efd;
			const int	one = 1;
			uint16_t	port;
			std::string	addr;
			std::thread	m_thread;

			void threadFunc();

		public:
			Server(std::string addr, uint16_t port);
			~Server();

			void start();
			void stop();
			void join();
	};
}

