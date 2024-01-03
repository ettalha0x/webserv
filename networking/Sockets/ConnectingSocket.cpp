#include "ConnectingSocket.hpp"

ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long interface) : Socket(domain, service, protocol, port, interface) {
	int connection = connect_to_network(get_socket(), get_address());
	test_connection(connection);
}

int  ConnectingSocket::connect_to_network(int sock, struct sockaddr_in address) {
	return connect(sock, (struct sockaddr *)&address, sizeof(address));
};

ConnectingSocket::~ConnectingSocket() {
}