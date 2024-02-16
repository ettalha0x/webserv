#include "Server.hpp"

Server::Server(std::vector<t_server_config> &configs, int domain, int service, int protocol, u_long interface, int backlog) {
	for (size_t i = 0; i < configs.size(); i++) {
		server_sockets.push_back(new ListeningSocket(domain, service, protocol, configs[i].port, interface, backlog));
	}
}

std::vector<ListeningSocket*>	Server::get_server_sock() {
	return server_sockets;
}

Server::~Server() {
	// delete socket;
}
