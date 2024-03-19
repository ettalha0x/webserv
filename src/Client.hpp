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
	public:
		std::pair<u_long, int> ipAndPort;
		bool			resGenerated;
		Client();
		Client(int fd, std::pair<u_long, int> ipAndPort);
		pollfd& 		getPollfd();
		std::string&	getStringReq();
		HttpRequest&	getRequest();
		std::string&	getStringRes();
		HttpResponse&	getResponse();
		~Client();
		void			clearData(void);

		Client &		operator=( const Client& rhs );

	private:

};