#pragma once

# include <sys/poll.h>
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

class Client
{
	private:
		pollfd			socket;
		std::string		stringReq;
		HttpRequest		request;
		std::string		stringRes;
		HttpResponse	response;
		u_long			ip;
		int				port; 
	public:
		bool			resGenerated;
		Client();
		Client(int fd, u_long ip, int port);
		pollfd& 		getPollfd();
		std::string&	getStringReq();
		HttpRequest&	getRequest();
		std::string&	getStringRes();
		HttpResponse&	getResponse();
		~Client();

		Client &		operator=( const Client& rhs );

	private:

};