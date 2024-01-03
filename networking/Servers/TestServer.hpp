# pragma once

# include "Server.hpp"

class TestServer:  public Server {
	private:
		char buffer[30000];
		int new_socket;
	public:

		TestServer();
		void	launch();
		void	accepter();
		void	handler();
		void	responder();
		~TestServer();

};