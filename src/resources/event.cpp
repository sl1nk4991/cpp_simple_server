#include "resources/event.h"
#include <cerrno>
#include <cstring>
#include <sys/eventfd.h>
#include <system_error>
#include <unistd.h>

namespace Resources {
    Event::Event() {
        this->fd = eventfd(0, 0);
    }

    Event::~Event() {
        if (this->fd != -1) {
            close(this->fd);
        }
    }

    void Event::Write() {
        int ret = write(this->fd, &this->one, sizeof(this->one));
        if (ret == -1) {
            throw std::system_error(errno, std::system_category(), strerror(errno));
        }
    }

    int Event::getFd() {
        return this->fd;
    }
}
