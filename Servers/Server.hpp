# pragma once

# include <unistd.h>
# include "../Webserv.hpp"

class Server
{
	private:

		ListeningSocket *socket;
	public:

		Server(int domain, int service, int protocol, int port, u_long interface, int backlog);
		virtual void accepter() = 0;
		virtual void handler() = 0;
		virtual void responder() = 0;
		virtual void launch() = 0;
		ListeningSocket *get_sock();
		~Server();
};