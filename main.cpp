#include "server.h"

int main(int argc, char *argv[]) {
    tmp::Server obj("127.0.0.1", 8080);

    obj.join();

    return 0;
}
