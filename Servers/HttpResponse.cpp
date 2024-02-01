# include "Utils/Utils.hpp"
# include "HttpResponse.hpp"



HttpResponse::HttpResponse(t_server_config &config, HttpRequest &request) :request(request), config(config) {
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

std::string	HttpResponse::getStatusMessage(int	statusCode) {
	switch (statusCode) {
		case 200:
			return "OK";
		case 400:
			return "Bad Request";
		case 404:
			return "Not Found";
		case 500:
			return "Internal Server Error";
		default:
			return "Unknown Status";
	}
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
	setStatusMessage(getStatusMessage(getStatusCode()));
	addHeader("Content-Type", request.GetContentType());
	// addHeader("Content-Length", std::to_string(request.GetContentLength()));
    addHeader("Server", "Wind City Warrior's Web Server");
    addHeader("Date", getCurrentTimeInGMT());
	headerString = getHeaderString();
}

void	HttpResponse::constructBody() {
	std::string Path;
	struct stat st;
	if (request.GetPath() == "/") {
		Path = config.rootDir + "/" + config.indexFile[0];
	} else {
		Path = config.rootDir + request.GetPath();
		if (stat(Path.c_str(), &st))
			Path = "Sites-available/Error-pages/404-Not-Found.html";
	}
	body = getFileContent(Path);
}

std::string	HttpResponse::getHeader() {
	return headerString;
}

std::string	HttpResponse::getBody() {
	return body;
}

HttpResponse::~HttpResponse() {
}
