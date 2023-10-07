#pragma once

#include <cstdint>

namespace Resources {
    class Event {
        protected:
            int fd;
            const uint64_t one = 1;

        public:
            Event();
            Event(const Event&) = delete;
            Event(Event&&) = delete;
            Event& operator=(const Event&) = delete;
            Event& operator=(Event&&) = delete;
            ~Event();

            void Write();
            int getFd();
    };
}
