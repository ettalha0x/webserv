#include "Client.hpp"

Client::Client() {}

Client::Client(int fd, std::pair<u_long, int> ipAndPort) : resGenerated(false) {
    stringReq = "";
    stringRes = "";
	socket.fd = fd;
	socket.events = POLLIN | POLLOUT;
	socket.revents = 0;
    this->ipAndPort = ipAndPort;
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
        this->ipAndPort = rhs.ipAndPort;
    }
    return *this;
}

void Client::clearData(void) {
    stringReq.clear();
    stringRes.clear();
    resGenerated = false;
    request.clearData();
    response.clearData();
}

Client::~Client() {
}