#include "Socket.hpp"

// Default constructor
Socket::Socket(int domain, int service, int protocol, int port, u_long interface) {
	// define address struct
	address.sin_family = domain;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(interface);
	// establish socket
	sock = socket(domain, service, protocol);
	setNonBlocking(sock);
	int reuseaddr = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)))
        throw std::runtime_error("Error of binding by reusing address");
	test_connection(sock);
}

void	Socket::setNonBlocking(int sock) {
	// use fcntl to set non-blocking mode
	int flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}

// test connection virtual function
void	Socket::test_connection(int item_to_test) {
	// confirm ither the socket or connection has been successfully established
	if (item_to_test < 0)
	{
		perror("Failed to connect!");
		exit(EXIT_FAILURE);
	}
}

// Getter functions
struct	sockaddr_in Socket::get_address() {
	return address;
};

int		Socket::get_socket() {
	return sock;
};

// distructor
Socket::~Socket() {
}