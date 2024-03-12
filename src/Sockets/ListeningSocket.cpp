#include "ListeningSocket.hpp"

ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long host, int backlog) : BindingSocket(domain, service, protocol, port, host) {
	_backlog = backlog;
	listening = listen(get_socket(), _backlog);
	if (listening < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
}

ListeningSocket::~ListeningSocket() {
}