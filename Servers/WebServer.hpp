# pragma once

# include <sys/poll.h>
# include "Server.hpp"
# include "HttpResponse.hpp"
# define BUFFER_SIZE 1024 * 4

typedef std::map<int , HttpRequest> RequestContainer;
typedef std::map<int , HttpResponse> ResponseContainer;
typedef std::map<int , std::string> stringReqContainer;
typedef std::map<int , std::string> stringResContainer;

class WebServer:  public Server {
	private:
		std::vector<t_server_config>	configs;
		std::vector<pollfd> 			server_sockets;
		std::vector<pollfd>				client_sockets;
		stringReqContainer				stringRequests;
		stringResContainer				stringResponses;
		RequestContainer				Requests;
		bool							resGenerated;
		// ResponseContainer	responses;
		int								accepter(int &serverIndex);
		void							handler(int	&fdIndex);
		bool							responder(int &fdIndex);
		char							buffer[BUFFER_SIZE];

	public:
		WebServer(std::vector<t_server_config>	&configs);
		void							init_pollfd();
		void							launch();
		~WebServer();
};
