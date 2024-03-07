#ifndef UPLOAD_HPP
#define UPLOAD_HPP


#include "Servers/WebServer.hpp"
#include "Servers/HttpRequest.hpp"
#include "cgi.hpp"


int	upload(HttpRequest request, std::string path);
int		check_upload(std::string Body);


#endif