/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouchaad <aouchaad@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 22:07:02 by aouchaad          #+#    #+#             */
/*   Updated: 2024/01/13 18:36:04 by aouchaad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest() : _method("GET"), bodyExist(false) {}
HttpRequest::HttpRequest(std::string method) : _method(method), bodyExist(false){}

HttpRequest::~HttpRequest() {}

void HttpRequest::SetRequestLine(std::string requestLine) {
	this->_RequestLine = requestLine;
}
void HttpRequest::SetPath(std::string Path) {
	this->_path = Path;
}
void HttpRequest::SetHttpVersion(std::string HttpVersion) {
	this->_HttpVersion = HttpVersion;
}
void HttpRequest::SetHost(std::string Host) {
	this->_Host = Host;
}

void HttpRequest::UpDateStatus(int status) {
	this->Status = status;
}

// void HttpRequest::HeaderIsCompleted(void) {
// 	this->HeaderCompleted = true;
// }
void HttpRequest::BodyDoExist(void) {
	this->bodyExist = true;
}
// void HttpRequest::BodyIsCompleted(void) {
// 	this->bodyCompleted = true;
// }

std::string HttpRequest::GetRequestLine(void) const {
	return this->_RequestLine;
}
std::string HttpRequest::GetPath(void) const {
	return this->_path;
}
std::string HttpRequest::GetMethod(void) const {
	return this->_method;
}
std::string HttpRequest::GetHttpVersion(void) const {
	return this->_HttpVersion;
}
std::string HttpRequest::GetHost(void) const {
	return this->_Host;
}

HeaderContainer HttpRequest::GetHeaders(void) const {
	return this->Headers;
}
bodyContainer HttpRequest::GetBody(void) const {
	return this->body;
}
int HttpRequest::GetStatus(void) const {
	return this->Status;
}

// void findAndReplace(std::string &str) {
// 	size_t pos = 0;
// 	std::string newWord = "HELLO";
// 	std::string oldWord = "\r\n\r\n";
// 	while ((pos = str.find(oldWord, pos)) != str.npos) {
// 		std::cout << "KKEKEKE" << std::endl;
// 		str.replace(pos, oldWord.length(), newWord);
// 		pos += newWord.length();
// 	}
// }

void HttpRequest::header_parser(std::string request) {
	// findAndReplace(request);
	size_t pos = request.find("\n");
	this->_RequestLine = request.substr(0, pos);
	pos = this->_RequestLine.find(" ");
	this->_method = this->_RequestLine.substr(0, pos);
	pos++;
	size_t endPos = this->_RequestLine.find(" ", pos);
	this->_path = this->_RequestLine.substr(pos, endPos - pos);
	endPos++;
	this->_HttpVersion = this->_RequestLine.substr(endPos, _RequestLine.length() - endPos);
}

void HttpRequest::parser(std::string request) {
	header_parser(request);
}

std::ostream& operator<<(std::ostream& os, const HttpRequest& obj) {
	os << "####################################################" << std::endl;
	os << "Request Line : " << obj.GetRequestLine() << std::endl;
	os << "method : " << obj.GetMethod() << std::endl;
	os << "path : " << obj.GetPath() << std::endl;
	os << "http version : " << obj.GetHttpVersion() << std::endl;
	os << "####################################################" << std::endl;
	return os;
}