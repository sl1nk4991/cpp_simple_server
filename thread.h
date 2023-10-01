#pragma once

#include <thread>

namespace tmp {
	class Thread {
		protected:
			int efd = -1;
			std::thread m_thread;
			bool m_terminate = false;

			virtual void threadFunc() = 0;

		public:
			virtual void join() = 0;
			virtual void terminate() = 0;
	};
}
