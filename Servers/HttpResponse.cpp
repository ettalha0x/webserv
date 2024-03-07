# include "Utils/Utils.hpp"
# include "HttpResponse.hpp"



HttpResponse::HttpResponse() {
	
}

HttpResponse::HttpResponse(t_server_config &config, HttpRequest &request, std::string ID) :request(request), config(config), ID(ID) {
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
		case 201:
			return "Created";
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
		case 504:
			return "Gateway Timeout"; 
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
		// std::cout << "RUN CGI 1>>>>>>\n";
	struct stat st;
	if (Location.cgi_path.empty() || !extentionSuported(extention, Location))
	{
		setError(403, ERROR403);
		return;
	}
	if ((extention == ".php" && !stat((Location.cgi_path + "/php-cgi").c_str(), &st)) || (extention == ".py" && !stat((Location.cgi_path + "/python3").c_str(), &st)))
	{
		std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > resp;
		// std::cout << "RU~N CGI >>>>>>\n";
		cgi CGI(request, FinalPath, Location.cgi_path);
		// body = CGI.get_cgi_res();
		resp = CGI.get_cgi_res();
		// std::map<std::string , std::string>::iterator ite = resp.first.begin();
		// std::cout << YELLOW << "first == {" << std::endl;
		// while (ite != resp.first.end())
		// {
		// 	std::cout << "key = " << ite->first << " value == " << ite->second << std::endl;
		// 	ite++;
		// }
		// std::cout << "}" << RESET << std::endl;
		
		// std::cout << YELLOW << "second.first == {" <<  resp.second.first << "}" << RESET << std::endl;
		// std::cout << YELLOW << "second.second == {" <<  resp.second.second << "}" << RESET << std::endl;
		// std::cout << YELLOW << body << RESET << std::endl;
		if (resp.second.second == 500)
			setError(resp.second.second, ERROR500);
		if (resp.second.second == 504)
			setError(resp.second.second, ERROR504);
		if (resp.second.second == 200) {
			setStatusCode(200);
			if (!resp.first["Content-Type:"].empty())
				addHeader("Content-Type",resp.first["Content-Type:"]);
			if (!resp.first["Content-Length:"].empty())
				addHeader("Content-Length",resp.first["Content-Length:"]);
			body = resp.second.first;
		}
	}
}

std::string aliasHundler(location Location, std::string requestPath) {
	if (requestPath == "/")
		return Location.alias;
	requestPath.erase(0, Location.uri.size() - 1);
	return Location.alias + requestPath;
}

void HttpResponse::PostHundler(location Location) {

	// else {
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
					extention = getCgiExtension(FinalPath);
					if (extention == ".php" || extention == ".py") {
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
				extention = getCgiExtension(FinalPath);
					std::cout << extention << std::endl;
				if (extention == ".php" || extention == ".py") {
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

	if (!Location.upload_path.empty()) {
		if (!request.GetRequestedFile().empty()) {
			extention = getCgiExtension(FinalPath);
			if (extention == ".php" && extention == ".py")
				return;
		}
		int exitstatus;
		exitstatus = upload(request, Location.upload_path);
		if (exitstatus == 201)
		{
			setStatusCode(201);
			std::cout << YELLOW << "Upload ########\n" << RESET ;
		}
		else
			setError(500,ERROR500);
		// check response status code
	}
	// }
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
				extention = getCgiExtension(FinalPath);
				if (extention == ".php" || extention == ".py") {
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
			extention = getCgiExtension(FinalPath);
			if (extention == ".php" || extention == ".py") {
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
				extention = getCgiExtension(FinalPath);
				if (extention == ".php" || extention == ".py") {
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
			extention = getCgiExtension(FinalPath);
			if (extention == ".php" || extention == ".py") {
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
	std::string finalPath;
	std::string	uploadPath;
	std::cout << GREEN << "#####################" << RESET << std::endl;
	if ((!(request.GetHeaders()["Transfer-Encoding"].empty()) && request.GetHeaders()["Transfer-Encoding"] != "chunked") || request.GetHttpVersion() != "HTTP/1.1") {
		setError(501, ERROR501);
		return;

	}
	if (request.GetRequestURI().size() > 2048) {
		setError(414,ERROR414);
		return;
	}
	if (request.GetBodySize() > config.maxBodySize * MILLION) {
		setError(413,ERROR413);
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
	location Location;
	try
	{
		Location = getMatchedLocation(path);
		if (!Location.redirection.empty()) {
			setStatusCode(301);
			addHeader("location", Location.redirection);
			return;
		}
		check_method(Location);

		callHundlerBymethod(Location);
	}
	catch(const LocationNotFoundException& e)
	{
		std::cout << e.what() << std::endl;
		if (path == "Error-pages") {
			setStatusCode(404);
			addHeader("Content-Type", "text/html");
			body = ERROR404;
		} else
			setError(404,ERROR404);
		return;
	}
	catch (const NotAllowedException& e) {
		std::cout << e.what() << std::endl;
		setError(405,ERROR405);
		return;
	}
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
