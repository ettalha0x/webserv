# pragma once

# include <iostream>
# include <string>
# include "BindingSocket.hpp"

class ListeningSocket: public BindingSocket {
	private:
		int _backlog;
		int listening;
	public:

		ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int backlog);
		void start_listening();
		~ListeningSocket();
};
