# pragma once

# include <unistd.h>
# include <iostream>
# include <fstream>
# include <sys/stat.h>
# include <vector>
# include <map>
# include "Sockets/SocketLib.hpp"
# include "configFile.hpp"
# include "Client.hpp"

extern std::vector<std::pair<std::string, long> > cookie_vector_expe ;

# define BUFFER_SIZE 1024 * 400
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
		std::vector<std::pair<u_long, int> > ipAndPort;
		WebServer(std::vector<t_server_config>	&configs);
		std::vector<ListeningSocket> 	get_server_sock();
		void							check_delete_session();
		void							getClientsPollfds();
		void							setNonBlocking(int sock);
		void							init_pollfd();
		void							launch();
		~WebServer();
};