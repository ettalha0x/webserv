#include "Server.hpp"

Server::Server(std::vector<t_server_config> &configs, int domain, int service, int protocol, u_long interface, int backlog) {
	for (size_t i = 0; i < configs.size(); i++) {
		sockets.push_back(new ListeningSocket(domain, service, protocol, configs[i].port, interface, backlog));
	}
}

std::vector<ListeningSocket*>	Server::get_sock() {
	return sockets;
}

Server::~Server() {
	// delete socket;
}
