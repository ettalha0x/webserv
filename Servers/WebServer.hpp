# pragma once

# include "Server.hpp"
# include "Client.hpp"
# include "../session/session.hpp"
# define BUFFER_SIZE 1024 * 4

class Client;

// typedef std::map<int , HttpRequest> RequestContainer;
// typedef std::map<int , HttpResponse> ResponseContainer;
// typedef std::map<int , std::string> stringReqContainer;
// typedef std::map<int , std::string> stringResContainer;
// typedef std::map<int , bool>		resTracker;

class WebServer:  public Server {
	private:
		std::vector<t_server_config>	configs;
		std::vector<pollfd> 			server_sockets;
		std::vector<pollfd>				client_sockets;
		std::map<int, Client>			clients;
		// stringReqContainer				stringRequests;
		// stringResContainer				stringResponses;
		// RequestContainer				Requests;
		// resTracker						resGenerated;
		// ResponseContainer	responses;
		int								accepter(int &serverIndex);
		void							handler(int	&fd);
		bool							responder(int &fd);
		char							buffer[BUFFER_SIZE];
		std::string			ID;
	public:
		WebServer(std::vector<t_server_config>	&configs);
		void							init_pollfd();
		void							getClientsPollfds();
		void							launch();
		~WebServer();
};