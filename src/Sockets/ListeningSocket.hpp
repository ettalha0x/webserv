# pragma once

# include <iostream>
# include <string>
# include "BindingSocket.hpp"

class ListeningSocket: public BindingSocket {
	private:
		int _backlog;
		int listening;
	public:

		ListeningSocket(int domain, int type, int protocol, int port, u_long host, int backlog);
		virtual ~ListeningSocket();
};
