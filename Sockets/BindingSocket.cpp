#include "BindingSocket.hpp"

BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long interface) : Socket(domain, service, protocol, port, interface) {
	// establish network connection
	binding = connect_to_network(get_socket(), get_address());
	test_connection(binding);
}

int	BindingSocket::connect_to_network(int sock, struct sockaddr_in address) {
	return bind(sock, (struct sockaddr *)&address, sizeof(address));
}


BindingSocket::~BindingSocket() {
}