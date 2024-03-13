#include "Client.hpp"

Client::Client() {
}

Client::Client(int fd, u_long ip, int port) {
    stringReq = "";
    stringRes = "";
	socket.fd = fd;
	socket.events = POLLIN | POLLOUT;
	socket.revents = 0;
    this->ip = ip;
    this->port = port;
}

pollfd& 		Client::getPollfd() {
	return socket;
}

std::string&	Client::getStringReq() {
	return stringReq;
}

HttpRequest&	Client::getRequest() {
	return request;
}

std::string&	Client::getStringRes() {
	return stringRes;
}

HttpResponse&	Client::getResponse() {
	return response;
}

Client& Client::operator=(const Client& rhs) {
    if (this != &rhs) { 
        this->socket = rhs.socket;
        this->stringReq = rhs.stringReq;
        this->request = rhs.request;
        this->stringRes = rhs.stringRes;
        this->response = rhs.response;
        this->resGenerated = rhs.resGenerated;
        this->ip = rhs.ip;
        this->port = rhs.port;
    }
    return *this;
}

Client::~Client() {
}