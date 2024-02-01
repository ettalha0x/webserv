# pragma once

# include <unistd.h>
# include <iostream>
# include <fstream>
# include <sys/stat.h>
# include <vector>
# include <map>
# include "../Sockets/SocketLib.hpp"
# include "../config/configFile.hpp"

class Server
{
	private:

		std::vector<ListeningSocket *> sockets;
	public:

		Server(std::vector<t_server_config> &configs, int domain, int service, int protocol, u_long interface, int backlog);
		virtual void accepter(int &serverIndex) = 0;
		virtual void handler(int &fdIndex) = 0;
		virtual void responder(int &fdIndex) = 0;
		virtual void launch() = 0;
		std::vector<ListeningSocket*> get_server_sock();
		~Server();
};