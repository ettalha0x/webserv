#include "Socket.hpp"

Socket::Socket(int domain, int service, int protocol, int port, u_long interface) {
	address.sin_family = domain;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(interface);
	sock = socket(domain, service, protocol);
	test_connection(sock);
	setNonBlocking(sock);
	int reuseaddr = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)))
        perror("Error of binding by reusing address");
}

void	Socket::setNonBlocking(int sock) {
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0)
		perror("get flags error");
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK | O_CLOEXEC))
		perror("set to non-blocking error");
}

void	Socket::test_connection(int item_to_test) {
	if (item_to_test < 0) {
		perror("Failed to connect!");
		exit(EXIT_FAILURE);
	}
}

struct	sockaddr_in Socket::get_address() {
	return address;
};

int		Socket::get_socket() {
	return sock;
};

Socket::~Socket() {
}