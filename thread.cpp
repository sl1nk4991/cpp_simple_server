#include "thread.h"
#include <unistd.h>
#include <stdexcept>
#include <cstring>

namespace tmp {
	void Thread::join() {
		if (this->m_thread.joinable()) {
			this->m_thread.join();
		}
	}

	void Thread::terminate() {
		this->m_terminate = true;

		auto ret = write(this->efd, &this->one, sizeof(this->one));
		if (ret == -1) {
			throw std::runtime_error(strerror(errno));
		}
	}
}
