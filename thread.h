#pragma once

#include <cstdint>
#include <thread>

namespace tmp {
	class Thread {
		protected:
			int efd = -1;
			uint64_t one = 1;
			std::thread m_thread;
			bool m_terminate = false;

			virtual void start() = 0;
			virtual void threadFunc() = 0;

		public:
			void join();
			void terminate();
	};
}
