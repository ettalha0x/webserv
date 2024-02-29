#ifndef UPLOAD_HPP
#define UPLOAD_HPP


#include "Servers/WebServer.hpp"
#include "Servers/HttpRequest.hpp"
#include "cgi.hpp"


void	upload(HttpRequest request, std::string path);
void	check_upload(std::string Body);

#endif