# include "Utils/Utils.hpp"
# include "HttpResponse.hpp"



HttpResponse::HttpResponse() {
	
}

HttpResponse::HttpResponse(t_server_config &config, HttpRequest &request, std::string ID) :request(request), config(config), ID(ID) {
	constructBody();
	constructHeader();
	// std::string locationRoute;
	// std::string path;
	// path = request.GetPath();
	// locationRoute = getLocationRoute(path);
	// location location = getMatchedLocation(locationRoute);
	// std::cout << "location.upload_path ==>>>   " << location.upload_path << std::endl;
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
		case 301:
			return "Moved Permanently";
		case 400:
			return "Bad Request";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";
		case 413:
			return "Request Entity Too Large";
		case 414:
			return "URI Too Long";
		case 500:
			return "Internal Server Error";
		case 501:
			return "Not Implemented"; 
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
	if (request.GetRequestURI().size() > 2048) {
		setStatusCode(414);
		addHeader("Content-Type", "text/html");
		body = ERROR414;
		return;
	}
	// HeaderContainer::iterator it = request.GetHeaders().find("Transfer-Encoding");
	if ((!(request.GetHeaders()["Transfer-Encoding"].empty()) && request.GetHeaders()["Transfer-Encoding"] != "chunked") || request.GetHttpVersion() != "HTTP/1.1") {
		std::cout << RED << request << RESET << std::endl;
		std::cout << "=" << request.GetHttpVersion() << "=" << std::endl;
		std::cout << RED << "not implimented" << RESET << std::endl;
		setStatusCode(501);
		addHeader("Content-Type", "text/html");
		if (!config.Errors[501].empty())
			body = getFileContent(config.Errors[501]);
		else
			body = ERROR501;
		return;

	}
	if (request.GetBodySize() > config.maxBodySize * MILLION) {
		std::cout << "body too large" << std::endl;
		setStatusCode(413);
		addHeader("Content-Type", "text/html");
		if (!config.Errors[413].empty())
			body = getFileContent(config.Errors[413]);
		else
			body = ERROR413;
		return;
	}
	location Location;
	try
	{
		Location = getMatchedLocation(locationRoute);
		check_method(Location);
		upload(request, Location.upload_path);
		if (!Location.redirection.empty()) {
			setStatusCode(301);
			addHeader("location", Location.redirection);
			return;
		}
		std::string file = request.GetRequestedFile();
		if (file.empty())
			file = Location.index;
		finalPath =  Location.root + path + file;
		setStatusCode(200);
		if (stat(finalPath.c_str(), &st) || file.empty()) {
			uploadPath =  Location.upload_path + "/" + file;
			if (stat(uploadPath.c_str(), &st) || file.empty()) {
				if (!stat(uploadPath.c_str(), &st) && file.empty() && Location.autoIndex) {
					std::cout << "AutoIndex" << std::endl;
					setStatusCode(200);
					addHeader("Content-Type", GetFileExtension(finalPath));
					body = list_dir(finalPath.substr(0, finalPath.find_last_of('/')));
					return;
				}
				else {
					addHeader("Content-Type", "text/html");
					if (!config.Errors[404].empty()) {
						setStatusCode(301);
						addHeader("location", config.Errors[404]);
					}
					else {
						setStatusCode(404);
						body = ERROR404;
					}
					return;
					// finalPath = "Sites-available/Error-pages/404-Not-Found.html";
				}
			} else {
				finalPath = uploadPath;
			}
		}

	}
	catch(const LocationNotFoundException& e)
	{
		std::cout << e.what() << std::endl;
		setStatusCode(404);
		addHeader("Content-Type", "text/html");
		if (!config.Errors[404].empty())
			body = getFileContent(config.Errors[404]);
		else
			body = ERROR404;
		return;
		// finalPath = "Sites-available/Error-pages/404-Not-Found.html";
	}
	catch (const NotAllowedException& e) {
		std::cout << e.what() << std::endl;
		setStatusCode(405);
		addHeader("Content-Type", "text/html");
		if (!config.Errors[405].empty())
			body = getFileContent(config.Errors[405]);
		else
			body = ERROR405;
		return;
		// finalPath = "Sites-available/Error-pages/405-Method-Not-Allowed.html";
	}
	// std::cout << "finalPath: " << finalPath << std::endl;
	addHeader("Content-Type", GetFileExtension(finalPath));
	std::string exe = getCgiExtension(finalPath);
	if ((exe == "php" && !stat((Location.cgi_path + "/php-cgi").c_str(), &st)) || (exe == "py" && !stat((Location.cgi_path + "/python3").c_str(), &st)))
	{
		cgi CGI(request, finalPath, Location.cgi_path);
		body = CGI.get_cgi_res();
		std::cout << YELLOW << body << RESET << std::endl;
	}
	else if (finalPath == uploadPath && request.GetMethod() == "DELETE") {
		std::remove(finalPath.c_str());
	} else {
		body = getFileContent(finalPath);
	}
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
