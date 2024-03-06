#pragma once

# include <iostream>
# include <string>
# include <fcntl.h>
# include <cstdlib>
# include <cstdio>
# include <sys/socket.h>
# include <netinet/in.h>

class Socket
{
	private:
		struct sockaddr_in address;
		int sock;

	public:

		Socket(int domain, int service, int protocol, int port, u_long interface);
	
		virtual int  connect_to_network(int sock, struct sockaddr_in address) = 0;

		void	setNonBlocking(int sock);

		void	test_connection(int item_to_test);

		struct	sockaddr_in get_address();
		int		get_socket();
		
		virtual ~Socket();
 

};