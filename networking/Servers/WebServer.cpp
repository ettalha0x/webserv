#include "WebServer.hpp"

WebServer::WebServer(): Server(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10) {
	launch();
}

void WebServer::accepter() {
	struct sockaddr_in address = get_sock()->get_address();
	int		addrlen = sizeof(address);
	new_socket = accept(get_sock()->get_socket(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
	buffer = new char[1024];
	read(new_socket, buffer, 1024);
}

void WebServer::handler() {
	std::cout << "-------------- REQUSTE RECEVIED --------------" << std::endl;
	std::cout << buffer << std::endl;
}

void WebServer::responder() {
	std::string res = "Hello form server";
	write(new_socket, res.c_str(), res.length());
	close(new_socket);
}

void WebServer::launch() {
	while (true) {
		std::cout << "------- WAITING FOR INCOMMING REQUSTES -------" << std::endl;
		accepter();
		handler();
		responder();
		std::cout << "-------------- DONE --------------"<< std::endl;
	}
}

WebServer::~WebServer() {
}