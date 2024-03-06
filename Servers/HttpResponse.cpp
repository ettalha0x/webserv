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
		case 204:
			return "No Content";
		case 301:
			return "Moved Permanently";
		case 400:
			return "Bad Request";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";
		case 409:
			return "Conflict";
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
	// if (path == "/")
	// 	return path;
	// size_t lastSlash = path.find_last_of("/");
	// size_t beforLast = path.substr(0, lastSlash).find_last_of("/");
    return (path.substr(0, path.find_last_of("/") + 1));
}

void	HttpResponse::constructHeader(void) {
	setStatusMessage(getStatusMessage(getStatusCode()));
	addHeader("Content-Length", ft_to_string(GetContentLength()));
    addHeader("Server", "Wind City Warrior's Web Server");
    addHeader("Date", getCurrentTimeInGMT());
	headerString = getHeaderString();
}

void removeBlock(std::string &tmp) {
	if (tmp == "/")
		tmp.clear();
	else {
		size_t lastSlash = tmp.find_last_of("/");
		size_t beforLast = tmp.substr(0, lastSlash).find_last_of("/");
		tmp.erase(beforLast, lastSlash + 1);
	}
}

location	HttpResponse::getMatchedLocation(std::string &locationRoute) {
	std::string tmp = locationRoute;
	while (!tmp.empty()) {
		for (std::map<std::string, location>::iterator it = config.locations.begin(); it != config.locations.end(); ++it) {
			if (it->first == tmp) {
				return it->second;
			}
		}
		removeBlock(tmp);
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

void HttpResponse::setError(int errorNum, std::string error) {
	addHeader("Content-Type", "text/html");
	if (!config.Errors[errorNum].empty()) {
		setStatusCode(301);
		addHeader("location", config.Errors[errorNum]);
	}
	else {
		setStatusCode(errorNum);
		body = error;
	}
}

bool HttpResponse::extentionSuported(std::string exe, location Location) {
	for(size_t i = 0; i < Location.cgi_extentions.size(); i++) {
		if (exe == Location.cgi_extentions[i])
			return true;
	}
	return false;
}

void HttpResponse::callHundlerBymethod(location Location) {
	if (request.GetMethod() == "POST")
		PostHundler(Location);
	else if (request.GetMethod() == "DELETE")
		DeleteHundler(Location);
	else if (request.GetMethod() == "GET")
		GetHundler(Location);
}

void HttpResponse::runCGI(std::string extention, location Location) {
	if (Location.cgi_path.empty() || !extentionSuported(extention, Location)) {
		setError(403, ERROR403);
		return;
	}
	if ((extention == "php" && !stat((Location.cgi_path + "/php-cgi").c_str(), 0)) || (extention == "py" && !stat((Location.cgi_path + "/python3").c_str(), 0)))
	{
		cgi CGI(request, FinalPath, Location.cgi_path);
		body = CGI.get_cgi_res();
		// std::cout << YELLOW << body << RESET << std::endl;
	}
}

std::string aliasHundler(location Location, std::string requestPath) {
	if (requestPath == "/")
		return Location.alias;
	requestPath.erase(0, Location.uri.size() - 1);
	return Location.alias + requestPath;
}

void HttpResponse::PostHundler(location Location) {
	if (!Location.upload_path.empty()) {
		upload(request, Location.upload_path);
		// check response status code
	}
	else {
		struct stat st;
		std::string extention;
		if (Location.alias.empty())
			FinalPath = Location.root + request.GetPath() + request.GetRequestedFile();
		else
			FinalPath = aliasHundler(Location, request.GetPath()) + request.GetRequestedFile();
		if (stat(FinalPath.c_str(), &st) == 0) {
			if (S_ISDIR(st.st_mode)) {
				FinalPath += Location.index;
				if (stat(FinalPath.c_str(),&st) == 0) {
					extention = GetFileExtension(FinalPath);
					if (extention == "php" || extention == "py") {
					runCGI(extention, Location);
					// check of errors in cgi
					} else {
						setStatusCode(200);
						addHeader("Content-Type", "text/html");
						body = getFileContent(FinalPath);
					}
				} else
					setError(403,ERROR403);
			} else if (S_ISREG(st.st_mode)) {
				extention = GetFileExtension(FinalPath);
				if (extention == "php" || extention == "py") {
					runCGI(extention, Location);
					// check of errors in cgi
				}
				else {
					setStatusCode(200);
					addHeader("Content-Type", GetFileExtension(FinalPath));
					body = getFileContent(FinalPath);
				}
			}
		} else
			setError(404,ERROR404);
	}
}
void HttpResponse::DeleteHundler(location Location) {
	struct stat st;
	std::string extention;
	if (Location.alias.empty())
			FinalPath = Location.root + request.GetPath() + request.GetRequestedFile();
		else
			FinalPath = aliasHundler(Location, request.GetPath()) + request.GetRequestedFile();
	if (stat(FinalPath.c_str(), &st) == 0) {
		if (S_ISDIR(st.st_mode)) {
			FinalPath += Location.index;
			if (stat(FinalPath.c_str(),&st) == 0) {
				extention = GetFileExtension(FinalPath);
				if (extention == "php" || extention == "py") {
				runCGI(extention, Location);
				// check of errors in cgi
				} else {
					if (std::remove(FinalPath.c_str()) == 0)
						setStatusCode(204);
					else
						setError(403, ERROR403);
				}
			} else
				setError(403,ERROR403);
		} else if (S_ISREG(st.st_mode)) {
			extention = GetFileExtension(FinalPath);
			if (extention == "php" || extention == "py") {
				runCGI(extention, Location);
				// check of errors in cgi
			}
			else {
				if (std::remove(FinalPath.c_str()) == 0)
					setStatusCode(204);
				else
					setError(403, ERROR403);
			}
		}
	} else
		setError(404,ERROR404);
}
void HttpResponse::GetHundler(location Location) {
	struct stat st;
	std::string extention;
	if (Location.alias.empty())
		FinalPath = Location.root + request.GetPath() + request.GetRequestedFile();
	else
		FinalPath = aliasHundler(Location, request.GetPath()) + request.GetRequestedFile();
	if (stat(FinalPath.c_str(), &st) == 0) {
		if (S_ISDIR(st.st_mode)) {
			FinalPath = FinalPath + Location.index;
			if (stat(FinalPath.c_str(),&st) == 0) {
				extention = GetFileExtension(FinalPath);
				if (extention == "php" || extention == "py") {
				runCGI(extention, Location);
				// check of errors in cgi
				} else {
					setStatusCode(200);
					addHeader("Content-Type", "text/html");
					body = getFileContent(FinalPath);
				}
			} else if (Location.autoIndex) {
				setStatusCode(200);
				addHeader("Content-Type", GetFileExtension(FinalPath));
				body = list_dir(FinalPath.substr(0, FinalPath.find_last_of('/')));
			} else
				setError(403,ERROR403);
		} else if (S_ISREG(st.st_mode)) {
			extention = GetFileExtension(FinalPath);
			if (extention == "php" || extention == "py") {
				runCGI(extention, Location);
				// check of errors in cgi
			}
			else {
				setStatusCode(200);
				addHeader("Content-Type", GetFileExtension(FinalPath));
				body = getFileContent(FinalPath);
			}
		}
	} else
		setError(404,ERROR404);
}

void	HttpResponse::constructBody() {
	std::string path;
	std::string locationRoute;
	std::string finalPath;
	std::string	uploadPath;
	std::cout << GREEN << "#####################" << RESET << std::endl;
	if ((!(request.GetHeaders()["Transfer-Encoding"].empty()) && request.GetHeaders()["Transfer-Encoding"] != "chunked") || request.GetHttpVersion() != "HTTP/1.1") {
		// std::cout << RED << request << RESET << std::endl;
		// std::cout << "=" << request.GetHttpVersion() << "=" << std::endl;
		// std::cout << RED << "not implimented" << RESET << std::endl;
		setError(501, ERROR501);
		// setStatusCode(501);
		// addHeader("Content-Type", "text/html");
		// if (!config.Errors[501].empty())
		// 	body = getFileContent(config.Errors[501]);
		// else
		// 	body = ERROR501;
		return;

	}
	if (request.GetRequestURI().size() > 2048) {
		setError(414,ERROR414);
		// setStatusCode(414);
		// addHeader("Content-Type", "text/html");
		// body = ERROR414;
		return;
	}
	if (request.GetBodySize() > config.maxBodySize * MILLION) {
		// std::cout << "body too large" << std::endl;
		setError(413,ERROR413);
		// setStatusCode(413);
		// addHeader("Content-Type", "text/html");
		// if (!config.Errors[413].empty())
		// 	body = getFileContent(config.Errors[413]);
		// else
		// 	body = ERROR413;
		return;
	}
	path = request.GetPath();
	std::cout << RED << path << RESET << std::endl;
	if (path.find_last_of('/') != path.size() - 1) {
		if (request.GetMethod() == "DELETE")
			setError(409,ERROR409);
		else {
			setStatusCode(301);
			addHeader("location", path + "/");
		}
		return;
	}
	std::cout << YELLOW << "Path: " << path << RESET << std::endl;

	// locationRoute = getLocationRoute(path);
	locationRoute = path;
	std::cout << RED << locationRoute << RESET << std::endl;
	// std::cout << YELLOW << "Location Route: " <<locationRoute << RESET << std::endl;
	// HeaderContainer::iterator it = request.GetHeaders().find("Transfer-Encoding");
	location Location;
	try
	{
		Location = getMatchedLocation(locationRoute);
		if (!Location.redirection.empty()) {
			setStatusCode(301);
			addHeader("location", Location.redirection);
			return;
		}
		check_method(Location);

		callHundlerBymethod(Location);


		// upload(request, Location.upload_path);



		// std::string file = request.GetRequestedFile();
		// if (file.empty())
		// 	file = Location.index;
		// finalPath =  Location.root + locationRoute + file;
		// std::cout << "+++++" << finalPath << std::endl;
		// setStatusCode(200);
		// if (stat(finalPath.c_str(), &st) || file.empty()) {
		// 	uploadPath =  Location.upload_path + "/" + file;
		// 	if (stat(uploadPath.c_str(), &st) || file.empty()) {
		// 		if (!stat(uploadPath.c_str(), &st) && file.empty() && Location.autoIndex) {
		// 			std::cout << "AutoIndex" << std::endl;
		// 			setStatusCode(200);
		// 			addHeader("Content-Type", GetFileExtension(finalPath));
		// 			body = list_dir(finalPath.substr(0, finalPath.find_last_of('/')));
		// 			return;
		// 		}
		// 		else {
		// 			// addHeader("Content-Type", "text/html");
		// 			// if (!config.Errors[404].empty()) {
		// 			// 	setStatusCode(301);
		// 			// 	addHeader("location", config.Errors[404]);
		// 			// }
		// 			// else {
		// 			// 	setStatusCode(404);
		// 			// 	body = ERROR404;
		// 			// }
		// 			setError(404,ERROR404);
		// 			return;
		// 			// finalPath = "Sites-available/Error-pages/404-Not-Found.html";
		// 		}
		// 	} else {
		// 		finalPath = uploadPath;
		// 	}
		// }

	}
	catch(const LocationNotFoundException& e)
	{
		std::cout << e.what() << std::endl;
		// setStatusCode(404);
		// addHeader("Content-Type", "text/html");
		// if (!config.Errors[404].empty())
		// 	body = getFileContent(config.Errors[404]);
		// else
		// 	body = ERROR404;
		if (locationRoute == "Error-pages") {
			setStatusCode(404);
			addHeader("Content-Type", "text/html");
			body = ERROR404;
		} else
			setError(404,ERROR404);
		return;
		// finalPath = "Sites-available/Error-pages/404-Not-Found.html";
	}
	catch (const NotAllowedException& e) {
		std::cout << e.what() << std::endl;
		// setStatusCode(405);
		// addHeader("Content-Type", "text/html");
		// if (!config.Errors[405].empty())
		// 	body = getFileContent(config.Errors[405]);
		// else
		// 	body = ERROR405;
		setError(405,ERROR405);
		return;
		// finalPath = "Sites-available/Error-pages/405-Method-Not-Allowed.html";
	}
	// std::cout << "finalPath: " << finalPath << std::endl;
	// addHeader("Content-Type", GetFileExtension(finalPath));
	// std::string exe = getCgiExtension(finalPath);
	// if (exe == "php" || exe == "py") {
	// 	if (Location.cgi_path.empty() || !extentionSuported(exe, Location)) {
	// 		setError(403, ERROR403);
	// 		return;
	// 	}
	// 	if ((exe == "php" && !stat((Location.cgi_path + "/php-cgi").c_str(), &st)) || (exe == "py" && !stat((Location.cgi_path + "/python3").c_str(), &st)))
	// 	{
	// 		cgi CGI(request, finalPath, Location.cgi_path);
	// 		body = CGI.get_cgi_res();
	// 		// std::cout << YELLOW << body << RESET << std::endl;
	// 	}
	// }
	// else if (finalPath == uploadPath && request.GetMethod() == "DELETE") {
	// 	std::remove(finalPath.c_str());
	// } else {
	// 	body = getFileContent(finalPath);
	// }
}

std::string HttpResponse::GetFileExtension(std::string path){
	size_t pos = path.find_last_of(".");
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
