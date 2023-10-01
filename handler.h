#pragma once

#include "thread.h"

#define BUFFSIZE 1024

namespace tmp {
	class Handler: public Thread {
		private:
			int fd;

			std::string *recvMessage();
			void sendMessage(const std::string*);

			void start();
			void threadFunc();

		public:
			explicit Handler(int fd);
			~Handler();
	};
}
