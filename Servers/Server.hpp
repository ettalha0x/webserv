# pragma once

# include <unistd.h>
# include <iostream>
# include <fstream>
# include <vector>
# include <map>
# include "../Sockets/SocketLib.hpp"

class Server
{
	private:

		ListeningSocket *socket;
	public:

		Server(int domain, int service, int protocol, int port, u_long interface, int backlog);
		virtual void accepter() = 0;
		virtual void handler(int &fdIndex) = 0;
		virtual void responder(int &fdIndex) = 0;
		virtual void launch() = 0;
		ListeningSocket *get_sock();
		~Server();
};