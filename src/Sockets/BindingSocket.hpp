#pragma once

# include <iostream>
# include <string>
# include "Socket.hpp"

class BindingSocket: public Socket {
	private:
		int	binding;

	public:

		BindingSocket(int domain, int service, int protocol, int port, u_long host);
		~BindingSocket();

};

