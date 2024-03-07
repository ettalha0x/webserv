#pragma once

# include <iostream>
# include <string>
# include "Socket.hpp"

class BindingSocket: public Socket {
	private:
		int	binding;

	public:

		BindingSocket(int domain, int service, int protocol, int port, u_long interface);
		int  connect_to_network(int sock, struct sockaddr_in address);
		~BindingSocket();

};

