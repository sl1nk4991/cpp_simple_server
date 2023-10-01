#pragma once

#include "thread.h"
#include <string>

#define TIMEOUT			1000
#define MAX_CONS		100
#define MAX_BUFF_LENGTH	4096

namespace tmp {
	class Server: public Thread {
		private:
			uint16_t	port;
			std::string	addr;

			void start();
			void threadFunc();

		public:
			Server(std::string addr, uint16_t port);
			~Server();
	};

	extern "C" void signalHandler(int sig);
	static Server *sig_obj;
}

