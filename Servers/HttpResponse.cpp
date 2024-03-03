# include "Utils/Utils.hpp"
# include "HttpResponse.hpp"



HttpResponse::HttpResponse() {
	
}

HttpResponse::HttpResponse(t_server_config &config, HttpRequest &request, std::string ID) :request(request), config(config), ID(ID) {
	constructBody();
	constructHeader();
	std::string locationRoute;
	std::string path;
	path = request.GetPath();
	locationRoute = getLocationRoute(path);
	location location = getMatchedLocation(locationRoute);
	std::cout << "location.upload_path ==>>>   " << location.upload_path << std::endl;
	upload(request, location.upload_path);
}

void	HttpResponse::setStatusCode(int statusCode) {
	this->statusCode = statusCode;
}

void	HttpResponse::setStatusMessage(std::string statusMessage) {
	this->statusMessage = statusMessage;
}

 size_t		HttpResponse::GetContentLength() {
	return body.size();
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
		case 301:
			return "Moved Permanently";
		case 413:
			return "Request Entity Too Large";
		default:
			return "Unknown Status";
	}
}

std::string HttpResponse::getHeaderString() const {

	std::string headerString = "HTTP/1.1 " + ft_to_string(statusCode) + " " + statusMessage + "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++) {
		headerString += it->first + ": " + it->second + "\r\n";
	}
	HeaderContainer tmp = request.GetHeaders();
	if (tmp.find("Referer") == tmp.end())
	{
		headerString += "Set-cookie: sessionId=" + this->ID + "; Max-Age=120 \r\n";
	}
	return headerString + "\r\n";
}


std::string	HttpResponse::getLocationRoute(std::string &path) {
    return (path.substr(0, path.find_last_of('/') + 1));
}

void	HttpResponse::constructHeader(void) {
	setStatusMessage(getStatusMessage(getStatusCode()));
	addHeader("Content-Length", ft_to_string(GetContentLength()));
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
	throw LocationNotFoundException();
}

void HttpResponse::check_method(location Location) {
	for (size_t i = 0; i < Location.acceptedMethods.size(); i++) {
		if (request.GetMethod() == Location.acceptedMethods[i])
			return;
	}
	throw NotAllowedException();
}

void	HttpResponse::constructBody() {
	std::string path;
	std::string locationRoute;
	std::string finalPath;
	std::string	uploadPath;
	struct stat st;

	path = request.GetPath();
	if (path.find_last_of('/') != path.size() - 1)
		path += "/";
	// std::cout << YELLOW << "Path: " << path << RESET << std::endl;

	locationRoute = getLocationRoute(path);

	std::cout << YELLOW << "Location Route: " <<locationRoute << RESET << std::endl;
	if (request.GetBodySize() > config.maxBodySize * MILLION) {
		std::cout << "body too large" << std::endl;
		setStatusCode(413);
		addHeader("Content-Type", "text/html");
		body = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Request Body Too Large</title></head><body><h1>Request Body Too Large</h1></body></html>";
		return;
	}
	try
	{
		location location = getMatchedLocation(locationRoute);
		check_method(location);
		if (!location.redirection.empty()) {
			setStatusCode(301);
			addHeader("location", location.redirection);
			return;
		}
		std::string file = request.GetRequestedFile();
		if (file.empty())
			file = location.index;
		finalPath =  location.root + path + file;
		setStatusCode(200);
		if (stat(finalPath.c_str(), &st) || file.empty()) {
			uploadPath =  location.upload_path + "/" + file;
			if (stat(uploadPath.c_str(), &st) || file.empty()) {
				if (!stat(uploadPath.c_str(), &st) && file.empty() && location.autoIndex) {
					std::cout << "AutoIndex" << std::endl;
					setStatusCode(200);
					addHeader("Content-Type", GetFileExtension(finalPath));
					body = list_dir(finalPath.substr(0, finalPath.find_last_of('/')));
					return;
				}
				else {
					setStatusCode(404);
					finalPath = "Sites-available/Error-pages/404-Not-Found.html";
				}
			} else {
				finalPath = uploadPath;
			}
		}

	}
	catch(const LocationNotFoundException& e)
	{
		std::cout << e.what() << std::endl;
		finalPath = "Sites-available/Error-pages/404-Not-Found.html";
	}
	catch (const NotAllowedException& e) {
		std::cout << e.what() << std::endl;
		finalPath = "Sites-available/Error-pages/405-Method-Not-Allowed.html";
	}
	// std::cout << "finalPath: " << finalPath << std::endl;
	addHeader("Content-Type", GetFileExtension(finalPath));
	std::string exe = getCgiExtension(finalPath);
	if ((exe == "php" && !stat((config.cgiPath + "/php-cgi").c_str(), &st)) || (exe == "py" && !stat((config.cgiPath + "/python3").c_str(), &st)))
	{
		cgi CGI(request, finalPath, config.cgiPath);
		body = CGI.get_cgi_res();
		std::cout << YELLOW << body << RESET << std::endl;
	}
	else
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
