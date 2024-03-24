#include "Socket.hpp"

Socket::Socket(int domain, int type, int protocol, int port, u_long host) {
	address.sin_family = domain;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = host;
	sock = socket(domain, type, protocol);
	if (sock < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	setNonBlocking(sock);
	int reuseaddr = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)))
        perror("Error of binding by reusing address");
}

void	Socket::setNonBlocking(int sock) {
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) {
		perror("get flags error");
        close(sock);
    }
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK | O_CLOEXEC)) {
		perror("set to non-blocking error");
        close(sock);
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