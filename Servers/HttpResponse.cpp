# include "Utils/Utils.hpp"
# include "HttpResponse.hpp"



HttpResponse::HttpResponse(HttpRequest request) {
	(void)request;
	constructHeader();
	constructBody();
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

int	HttpResponse::getStatusCode() {
	return statusCode;
}

std::string	HttpResponse::getStatusMessage() {
	return statusMessage;
}

std::string HttpResponse::getHeaderString() const {

	std::string headerString = "HTTP/1.1 " + std::to_string(statusCode) + " " + statusMessage + "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++) {
		headerString += it->first + ": " + it->second + "\r\n";
	}
	return headerString + "\r\n";
}

void	HttpResponse::constructHeader(void) {
	setStatusCode(200);
	setStatusMessage("OK");
	addHeader("Content-Type", "text/html");
	// newResponse.addHeader("Content-Length", "1337");
    addHeader("Server", "Wind City Warrior's Web Server");
    addHeader("Date", getCurrentTimeInGMT());
	headerString = getHeaderString();
}

void	HttpResponse::constructBody(void) {
	body = getFileContent("/Users/nettalha/Desktop/webserv/Sites-available/Default/index.html");
}

std::string	HttpResponse::getHeader() {
	return headerString;
}

std::string	HttpResponse::getBody() {
	return body;
}

HttpResponse::~HttpResponse() {
}
