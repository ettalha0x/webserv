#include "TestServer.hpp"

TestServer::TestServer(): Server(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10) {
	launch();
}

void TestServer::accepter() {
	struct sockaddr_in address;
	int		addrlen;
	address = get_socket()->get_address();
	addrlen = sizeof(address);
	new_socket = accept(get_socket()->get_socket(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
	read(new_socket, buffer, 30000);
}

void TestServer::handler() {
	std::cout << buffer << std::endl;
}

void TestServer::responder() {
	std::string res = "Hello form server";
	write(new_socket, res.c_str(), strlen(res.c_str()));
	close(new_socket);
}

void TestServer::launch() {
	while (true) {
		std::cout << "------- WAITING -------" << std::endl;
		accepter();
		handler();
		responder();
		std::cout << "------- DONE -------" << std::endl;
	}
}

TestServer::~TestServer() {
}