#pragma once

# include <iostream>
# include <unistd.h>
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

		Socket(int domain, int type, int protocol, int port, u_long host);
	
		void	setNonBlocking(int sock);

		struct	sockaddr_in get_address();

		int		get_socket();
		
		virtual ~Socket();
 

};