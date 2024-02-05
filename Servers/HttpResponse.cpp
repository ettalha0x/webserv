# include "Utils/Utils.hpp"
# include "HttpResponse.hpp"



HttpResponse::HttpResponse(t_server_config &config, HttpRequest &request) :request(request), config(config) {
	constructBody();
	constructHeader();
}

void	HttpResponse::setStatusCode(int statusCode) {
	this->statusCode = statusCode;
}

void	HttpResponse::setStatusMessage(std::string statusMessage) {
	this->statusMessage = statusMessage;
}

 size_t		HttpResponse::GetContentLength() {
	return body.length();
 };

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
	setStatusMessage(getStatusMessage(getStatusCode()));
	addHeader("Content-Length", std::to_string(GetContentLength()));
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
		setStatusCode(200);
		if (stat(Path.c_str(), &st))
		{
			setStatusCode(404);
			Path = "Sites-available/Error-pages/404-Not-Found.html";
		}
	}
	addHeader("Content-Type", GetFileExtension(Path));
	body = getFileContent(Path);
}

std::string HttpResponse::GetFileExtension(std::string path){
	size_t pos = path.find(".");
	if (pos != path.npos)
		return  "text/" + path.substr(pos + 1, path.length() - (pos + 1));
	return  ("text/html");
}

std::string	HttpResponse::getHeader() {
	return headerString;
}

std::string	HttpResponse::getBody() {
	return body;
}

HttpResponse::~HttpResponse() {
}
