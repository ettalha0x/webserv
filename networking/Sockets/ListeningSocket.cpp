#include "ListeningSocket.hpp"

ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int backlog) : BindingSocket(domain, service, protocol, port, interface) {
	_backlog = backlog;
	start_listening();
	test_connection(listening);
}

void ListeningSocket::start_listening() {
	listening = listen(get_socket(), _backlog);
}

ListeningSocket::~ListeningSocket() {
}