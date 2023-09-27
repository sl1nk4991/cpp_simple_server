#include <asm-generic/socket.h>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <vector>
#include <map>
#include <iostream>

#define MAX_CONS 100
#define MAX_BUFF_LENGTH 4096

namespace test {
	class Server {
		private:
			std::string					addr;
			uint16_t					port;
			uint32_t					fd;
			sockaddr_in					server_sock;
			std::vector<uint32_t>		client_fds;
			std::map<sockaddr_in*, socklen_t*> client_socks;

			void handleConnection(int fd) {
				std::cout << "Client connected" << std::endl;
				std::vector<char> buff(MAX_BUFF_LENGTH);
				std::string msg;
				recv(fd, &buff[0], buff.size(), 0);
				shutdown(fd, SHUT_RDWR);
				msg.append(buff.cbegin(), buff.cend());

				std::cout << msg << std::endl;
			}
		public:
			Server(std::string addr, uint16_t port) {
				this->addr = addr;
				this->port = port;
			}

			void Start() {
				fd = socket(AF_INET, SOCK_STREAM, 0);
				if(fd == -1) {
					throw std::runtime_error("can't create socket");
				}

				if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, new int {1}, sizeof(int)) != 0 ) {
					throw std::runtime_error("can't set socket options");
				}

				server_sock.sin_family = AF_INET;
				server_sock.sin_port = htons(port);
				if(inet_aton(addr.c_str(), &server_sock.sin_addr) != 1) {
					throw std::runtime_error("can't use this address");
				}

				if(bind(fd, (sockaddr *)&server_sock, sizeof server_sock) != 0) {
					throw std::runtime_error("can't bind socket");
				}

				if(listen(fd, MAX_CONS) != 0) {
					throw std::runtime_error("can't listen socket");
				}

				int cfd;
				sockaddr_in *client_sock;
				socklen_t *client_sock_len;
				for(;;) {
					client_sock = new struct sockaddr_in;
					client_sock_len = new socklen_t {sizeof *client_sock};
					cfd = accept(fd, (sockaddr*)client_sock, client_sock_len);
					if(cfd == -1) {
						throw std::runtime_error("can't accept connection");
					}
					client_fds.emplace_back(cfd);
					client_socks.insert({client_sock, client_sock_len});
					handleConnection(cfd);
				}
			}
	};
}

