#pragma once

#include "server.h"
#include <string>
#include <thread>

namespace tmp {
	class Handler {
		private:
			std::thread m_thread;
			int fd = -1;
			bool m_terminate = false;

			std::string readMessage();
			void sendMessage(const std::string&);

			void stop();

		public:
			explicit Handler(int fd);
			~Handler();

			void terminate();
			void threadFunc();
	};
}
