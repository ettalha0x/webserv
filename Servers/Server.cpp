#include "Server.hpp"

Server::Server(t_server_config &config, int domain, int service, int protocol, u_long interface, int backlog) {
	for (size_t i = 0; i < config.port.size(); i++) {
		sockets.push_back(new ListeningSocket(domain, service, protocol, config.port[i], interface, backlog));
	}
}

std::vector<ListeningSocket*>	Server::get_sock() {
	return sockets;
}

Server::~Server() {
	// delete socket;
}
