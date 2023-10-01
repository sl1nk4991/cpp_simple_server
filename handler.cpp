#include "handler.h"
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <event2/event_compat.h>
#include <iostream>
#include <pthread.h>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <cstring>
#include <vector>
#include <sys/eventfd.h>
#include <unistd.h>

namespace tmp {
	Handler::Handler(int fd): fd(fd) {
		assert(!this->m_thread.joinable());

		this->efd = eventfd(0, 0);
		if (this->efd == -1) {
			throw std::runtime_error(strerror(errno));
		}

		this->m_thread = std::thread([this]() { this->threadFunc(); });
		pthread_setname_np(this->m_thread.native_handle(), "Handler");
	}

	Handler::~Handler() {
		this->stop();
	}

	void Handler::stop() {
		if (this->m_thread.joinable()) {
			this->m_thread.join();
		}
	}

	std::string *Handler::recvMessage() {
		std::string *msg = new std::string(BUFFSIZE, '\0');

		int rb = recv(this->fd, msg->data(), msg->size(), 0);
		if(rb < 1) {
			msg->clear();
			return msg;
		}

		return msg;
	}

	void Handler::sendMessage(const std::string *msg) {
		int n = send(this->fd, msg->c_str(), msg->length(), 0);
		if (n != msg->size()) {
			std::cerr << "message size: " << msg->length() << " actual sended: " << n << std::endl;
		}
	}

	void Handler::terminate() {
		this->m_terminate = true;

		uint64_t one = 1;
		auto ret = write(this->efd, &one, sizeof(one));
		if (ret == -1) {
			throw std::runtime_error(strerror(errno));
		}
	}

	void Handler::join() {}

	void Handler::threadFunc() {
		std::vector<struct pollfd> fds = { {this->efd, POLLIN, 0}, {this->fd, POLLIN, 0} };

		while (!this->m_terminate) {
			int n = poll(fds.data(), fds.size(), -1);
			if (n > 0) {
				if (fds[0].revents) {
					std::cout << "handle thread terminated" << std::endl;
				} else if (fds[1].revents) {
					std::string *msg = recvMessage();
					if (*msg != "") {
						std::cout << "Messsage: " << *msg << std::endl;
						this->sendMessage(msg);
					}
				}
			}
		}

		shutdown(this->fd, SHUT_RDWR);
	}
}
