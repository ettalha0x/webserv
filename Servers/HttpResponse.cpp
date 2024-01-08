#include "HttpResponse.hpp"

HttpResponse::HttpResponse() {
}

void	HttpResponse::setStatusCode(int statusCode) {
	this->statusCode = statusCode;
}

void	HttpResponse::setStatusMessage(std::string statusMessage) {
	this->statusMessage = statusMessage;
}

void HttpResponse::addHeader(const std::string& key, const std::string& value) {
			headers[key] = value;
}

std::string HttpResponse::getHeaderString() const {

	std::string headerString = "HTTP/1.1 " + std::to_string(statusCode) + " " + statusMessage + "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++) {
		headerString += it->first + ": " + it->second + "\r\n";
	}
	headerString += "\r\nHello form server ";
	return headerString + "\r\n";
}
HttpResponse::~HttpResponse() {
}
