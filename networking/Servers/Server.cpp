#include "Server.hpp"

Server::Server(int domain, int service, int protocol, int port, u_long interface, int backlog) {
	socket = new ListeningSocket(domain, service, protocol, port, interface, backlog);
}

ListeningSocket 	*Server::get_socket() {
	return socket;
}

Server::~Server() {
	// delete socket;
}
