#include "BindingSocket.hpp"

BindingSocket::BindingSocket(int domain, int type, int protocol, int port, u_long host) : Socket(domain, type, protocol, port, host) {
	struct sockaddr_in address = get_address();
	int sock = get_socket();
	binding = bind(sock, (struct sockaddr *)&address, sizeof(address));
	if (binding < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}
}

BindingSocket::~BindingSocket() {
}