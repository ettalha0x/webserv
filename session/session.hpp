#ifndef SESSION_HPP
#define SESSION_HPP

#include "../Servers/HttpRequest.hpp"
#include "../Servers/Utils/Utils.hpp"
#include <iostream>


class session
{
	private:
		std::vector<std::string> id_session;
	public:
		session();
		~session();
		std::string	create_session(HttpRequest new_request);
};


#endif