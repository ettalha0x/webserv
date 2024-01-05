# pragma once

# include "Server.hpp"

class WebServer:  public Server {
	private:
		char	*buffer;
		int 	new_socket;
		void	accepter();
		void	handler();
		void	responder();

	public:
		WebServer();
		void	launch();
		~WebServer();

};