#pragma once

# include <iostream>
# include <string>
#include <sys/socket.h>
#include <netinet/in.h>

class Socket
{
	private:
		struct sockaddr_in address;
		int sock;
		int connection;

	public:

		// constructor
		Socket(int domain, int service, int protocol, int port, u_long interface);
	
		// virtual function to connect to a network
		virtual int  connect_to_network(int sock, struct sockaddr_in address) = 0;

		// function to test socket and connection
		void	test_connection(int item_to_test);

		// getter functions
		struct	sockaddr_in get_address();
		int		get_socket();
		int		get_connection();

		// setter functions

		void	set_connection(int connection);

		// distructor
		~Socket();
 

};