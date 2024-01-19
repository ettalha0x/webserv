# pragma once

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

# include "Server.hpp"
#include <sys/poll.h>

typedef std::map<int , HttpRequest> RequestContainer;
typedef std::map<int , HttpResponse> ResponseContainer;
typedef std::map<int , std::string> stringReqContainer;

class WebServer:  public Server {
	private:
		int					server_socket;
		std::vector<pollfd>	client_sockets;
		stringReqContainer	stringRequests;
		RequestContainer	Requests;
		// ResponseContainer	responses;
		void				accepter();
		void				handler(int	&fdIndex);
		void				responder(int &fdIndex);

	public:
		WebServer();
		void	launch();
		~WebServer();
};
