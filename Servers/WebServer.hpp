# pragma once

# include <sys/poll.h>
# include "Server.hpp"
# include "HttpResponse.hpp"

typedef std::map<int , HttpRequest> RequestContainer;
typedef std::map<int , HttpResponse> ResponseContainer;
typedef std::map<int , std::string> stringReqContainer;

class WebServer:  public Server {
	private:
		std::vector<t_server_config>		configs;
		int					server_socket;
		std::vector<pollfd>	client_sockets;
		stringReqContainer	stringRequests;
		RequestContainer	Requests;
		// ResponseContainer	responses;
		void				accepter(int &serverIndex);
		void				handler(int	&fdIndex);
		void				responder(int &fdIndex);
		char				buffer[1024];
	public:
		WebServer(std::vector<t_server_config>	&configs);
		std::vector<int>		init_pollfd();
		void	launch();
		~WebServer();
};
