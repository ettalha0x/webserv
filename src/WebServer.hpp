# pragma once

# include <unistd.h>
# include <iostream>
# include <fstream>
# include <sys/stat.h>
# include <vector>
# include <map>
# include "Sockets/SocketLib.hpp"
# include "configFile.hpp"
# include "session.hpp"
# include "Client.hpp"


# define BUFFER_SIZE 1024 * 4
# define MILLION 1000000

class Client;

class WebServer {
	private:
		std::vector<ListeningSocket> 	server_listening_sockets;
		std::vector<t_server_config>	configs;
		std::vector<pollfd> 			server_sockets;
		std::vector<pollfd>				client_sockets;
		std::map<int, Client>			clients;
		int								accepter(int &serverIndex);
		void							handler(int	&fd);
		bool							responder(int &fd);
		char							buffer[BUFFER_SIZE];
		std::string						ID;
	public:
		WebServer(std::vector<t_server_config>	&configs);
		std::vector<ListeningSocket> 	get_server_sock();
		void							getClientsPollfds();
		void							setNonBlocking(int sock);
		void							init_pollfd();
		void							launch();
		~WebServer();
};