#include "BindingSocket.hpp"

BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long interface) : Socket(domain, service, protocol, port, interface) {
	// establish network connection
	int connection = connect_to_network(get_socket(), get_address());
	set_connection(connection);
	test_connection(connection);
}

int	BindingSocket::connect_to_network(int sock, struct sockaddr_in address) {
	return bind(sock, (struct sockaddr *)&address, sizeof(address));
}


BindingSocket::~BindingSocket() {
}