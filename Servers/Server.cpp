#include "Server.hpp"

Server::Server(t_server_config &config, int domain, int service, int protocol, u_long interface, int backlog) {
		socket = new ListeningSocket(domain, service, protocol, config.port, interface, backlog);
}

ListeningSocket*	Server::get_sock() {
	return socket;
}

Server::~Server() {
	// delete socket;
}
