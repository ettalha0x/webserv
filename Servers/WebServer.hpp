# pragma once

# include <sys/poll.h>
# include "Server.hpp"
# include "HttpResponse.hpp"

typedef struct res {
	size_t		pos;
	std::string	res;
} t_res;

typedef std::map<int , HttpRequest> RequestContainer;
typedef std::map<int , HttpResponse> ResponseContainer;
typedef std::map<int , std::string>	stringReqContainer;
typedef std::map<int , t_res> stringResContainer;

class WebServer:  public Server {
	private:
		std::vector<t_server_config>		configs;
		int					server_socket;
		std::vector<pollfd>	client_sockets;
		stringReqContainer	stringRequests;
		stringResContainer stringResponses;
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
