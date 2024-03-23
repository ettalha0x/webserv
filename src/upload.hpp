#ifndef UPLOAD_HPP
#define UPLOAD_HPP


#include "WebServer.hpp"
#include "HttpRequest.hpp"
#include "cgi.hpp"


int	upload(HttpRequest &request, std::string &path);
int		check_upload(std::string &Body);


#endif