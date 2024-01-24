# pragma once

# include <unistd.h>
# include <iostream>
# include <fstream>
# include <vector>
# include <map>
# include "../Sockets/SocketLib.hpp"
# include "../config/configFile.hpp"

class Server
{
	private:

		ListeningSocket *socket;
	public:

		Server(t_server_config &config, int domain, int service, int protocol, u_long interface, int backlog);
		virtual void accepter() = 0;
		virtual void handler(int &fdIndex) = 0;
		virtual void responder(int &fdIndex) = 0;
		virtual void launch() = 0;
		ListeningSocket *get_sock();
		~Server();
};