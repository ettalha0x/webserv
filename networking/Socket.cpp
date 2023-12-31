#include "Socket.hpp"

// Default constructor
Socket::Socket(int domain, int service, int protocol, int port, u_long interface) {
	// define address struct
	address.sin_family = domain;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(interface);
	// establish socket
	sock = socket(domain, service, protocol);
	test_connection(sock);
}

// test connection virtual function
void	Socket::test_connection(int item_to_test) {
	// confirm ither the socket or connection has been successfully established
	if (item_to_test < 0)
	{
		perror("Failed to connect ...");
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

int		Socket::get_connection() {
	return connection;
};

//setter functions

void	Socket::set_connection(int connection) {
	this->connection = connection;
};

// distructor
Socket::~Socket() {
}