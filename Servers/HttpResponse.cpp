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


std::string	HttpResponse::getLocationRoute(std::string &path) {
    return (path.substr(0, path.find_last_of('/') + 1));
}

void	HttpResponse::constructHeader(void) {
	setStatusMessage(getStatusMessage(getStatusCode()));
	addHeader("Content-Length", std::to_string(GetContentLength()));
    addHeader("Server", "Wind City Warrior's Web Server");
    addHeader("Date", getCurrentTimeInGMT());
	headerString = getHeaderString();
}

location	HttpResponse::getMatchedLocation(std::string &locationRoute) {
	for (std::map<std::string, location>::iterator it = config.locations.begin(); it != config.locations.end(); ++it) {
		if (it->first == locationRoute) {
			return it->second;
		}
  	}
	throw std::exception();
}

void	HttpResponse::constructBody() {
	std::string path;
	std::string locationRoute;
	std::string finalPath;
	struct stat st;

	path = request.GetPath();
	locationRoute = getLocationRoute(path);

	try
	{
		location location = getMatchedLocation(locationRoute);
		std::string file = request.GetRequestedFile();
		if (file.empty())
			file = location.index;
		finalPath =  location.root + path + file;
		setStatusCode(200);
		if (stat(finalPath.c_str(), &st))
		{
			std::cout << "Location not found 1" << std::endl;
			setStatusCode(404);
			finalPath = "Sites-available/Error-pages/404-Not-Found.html";
		}

	}
	catch(const std::exception& e)
	{
		std::cout << "Location not found 2" << std::endl;
		finalPath = "Sites-available/Error-pages/404-Not-Found.html";
	}
	std::cout << "finalPath: " << finalPath << std::endl;
	addHeader("Content-Type", GetFileExtension(finalPath));
	body = getFileContent(finalPath);
}

std::string HttpResponse::GetFileExtension(std::string path){
	size_t pos = path.find(".");
	if (pos != path.npos)
		return GetExtensionPrefix(path.substr(pos + 1, path.length() - (pos + 1)));
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
