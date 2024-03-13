#include "BindingSocket.hpp"

BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long host) : Socket(domain, service, protocol, port, host) {
	struct sockaddr_in address = get_address();
	int sock = get_socket();
	binding = bind(sock, (struct sockaddr *)&address, sizeof(address));
	if (binding < 0) {
		perror("bind");
		std::cout << "host : " << host << std::endl;
		std::cout << "port: " << port << std::endl;
		// exit(EXIT_FAILURE);
	}
}

BindingSocket::~BindingSocket() {
}