# pragma once

# include <sys/poll.h>
# include "Server.hpp"
# include "HttpResponse.hpp"

typedef std::map<int , HttpRequest> RequestContainer;
typedef std::map<int , HttpResponse> ResponseContainer;
typedef std::map<int , std::string> stringReqContainer;

class WebServer:  public Server {
	private:
		t_server_config		config;
		int					server_socket;
		std::vector<pollfd>	client_sockets;
		stringReqContainer	stringRequests;
		RequestContainer	Requests;
		// ResponseContainer	responses;
		void				accepter();
		void				handler(int	&fdIndex);
		void				responder(int &fdIndex);

	public:
		WebServer(t_server_config	&config);
		void	launch();
		~WebServer();
};
