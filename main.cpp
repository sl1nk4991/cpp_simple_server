#include "server.cpp"

int main(int argc, char *argv[]) {
	test::Server obj("127.0.0.1", 8080);

	obj.Start();

	return 0;
}
