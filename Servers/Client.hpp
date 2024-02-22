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
		bool			resGenerated;
		Client();
		Client(int fd);
		// Client( Client const & src );
		pollfd& 		getPollfd();
		std::string&	getStringReq();
		HttpRequest&	getRequest();
		std::string&	getStringRes();
		HttpResponse&	getResponse();
		~Client();

		Client &		operator=( const Client& rhs );

	private:

};

// std::ostream &			operator<<( std::ostream & o, Client const & i );