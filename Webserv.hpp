# pragma once

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define RESET "\033[0m"

#define ERROR400 "<!DOCTYPE html><html><head><title>400 Bad Request</title></head><body><h1>400 Bad Request</h1><p>Your request could not be understood by the server.</p></body></html>"
#define ERROR404 "<!DOCTYPE html><html><head><title>404 Not Found</title><style>body {font-family: Arial, sans-serif;text-align: center;padding: 50px;}h1 {font-size: 36px;margin-bottom: 20px;}p {font-size: 18px;margin-bottom: 20px;}</style></head><body><h1>404 Not Found</h1><p>The page you are looking for could not be found.</p></body></html>"
#define ERROR405 "<!DOCTYPE html><html><head><title>405 Method Not Allowed</title></head><body><h1>405 Method Not Allowed</h1><p>The requested method is not allowed for the requested resource.</p></body></html>"
#define ERROR413 "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Request Body Too Large</title></head><body><h1>Request Body Too Large</h1></body></html>"
#define ERROR414 "<!DOCTYPE html><html><head><title>414 URI Too Long</title></head><body><h1>414 URI Too Long</h1><p>The server cannot process the request because the request URI is too long.</p></body></html>"
#define ERROR501 "<!DOCTYPE html><html><head><title>501 Not Implemented</title></head><body><h1>501 Not Implemented</h1><p>The server does not support the functionality required to fulfill the request.</p></body></html>"
#define ERROR403 "<!DOCTYPE html><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>403 Forbidden</title><style>body {font-family: Arial, sans-serif;background-color: #f5f5f5;margin: 0;padding: 0;}.container {text-align: center;margin-top: 100px;}h1 {color: #555;}p {color: #777;}</style></head><body><div class=\"container\"><h1>403 Forbidden</h1><p>Sorry, you don't have permission to access this resource.</p></div></body></html>"
#define ERROR409 "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>409 Conflict</title><style>body{font-family:Arial,sans-serif;background-color:#f5f5f5;margin:0;padding:0;}.container{text-align:center;margin-top:100px;}h1{color:#555;}p{color:#777;}</style></head><body><div class=\"container\"><h1>409 Conflict</h1><p>Sorry, there was a conflict detected while processing your request. Please try again later or contact support for assistance.</p></div></body></html>"

# include "Sockets/SocketLib.hpp"
# include "Servers/WebServer.hpp"
# include "config/configFile.hpp"