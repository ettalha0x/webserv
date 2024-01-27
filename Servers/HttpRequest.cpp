/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nettalha <nettalha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 22:07:02 by aouchaad          #+#    #+#             */
/*   Updated: 2024/01/27 18:47:07 by nettalha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include <unistd.h>

HttpRequest::HttpRequest() : _contentLength(0), bodyExist(false), isChunked(false){}

HttpRequest::~HttpRequest() {}


std::string HttpRequest::GetRequestLine(void) const {
	return this->_RequestLine;}
std::string HttpRequest::GetPath(void) const {
	return this->_path;}
std::string HttpRequest::GetMethod(void) const {
	return this->_method;}
std::string HttpRequest::GetHttpVersion(void) const {
	return this->_HttpVersion;}
std::string HttpRequest::GetHost(void) const {
	return this->_Host;}
HeaderContainer HttpRequest::GetHeaders(void) const {
	return this->Headers;}
std::string HttpRequest::GetBoundary(void) const {
	return this->_boundary;}
std::string HttpRequest::GetBody(void) const {
	return this->body;}
QueryContainer HttpRequest::GetQuerty(void) const {
	return this->Query;}
std::string HttpRequest::GetContentType(void) const {
	return this->_contentType;}
std::string HttpRequest::GetServerName(void) const {
	return this->_serverName;}
int HttpRequest::GetContentLength(void) const {
	return this->_contentLength;}
int HttpRequest::GetPort(void) const {
	return this->_port;}

bool HttpRequest::bodyExistOrNot(void) const{
	return this->bodyExist;}
bool HttpRequest::ChunkedOrNot(void) const{
	return this->isChunked;}


void HttpRequest::ckeckForQuery(void) {
	size_t pos;
	if ((pos = this->_path.find("?")) != this->_path.npos) {
		size_t andCarPos = pos;
		bool stop = false;
		while (1) {
			size_t equalCarPos = this->_path.find("=", andCarPos);
			std::string key = this->_path.substr(andCarPos + 1, (equalCarPos - andCarPos) - 1);
			andCarPos = this->_path.find("&", equalCarPos);
			if (andCarPos == this->_path.npos) {
				andCarPos = this->_path.length();
				stop = true;
			}
			std::string value = this->_path.substr(equalCarPos + 1, (andCarPos - equalCarPos) - 1);
			this->Query.push_back(std::make_pair(key, value));
			if (stop)
				break;
		}
		this->_path = this->_path.substr(0, pos);
	}
}

void HttpRequest::read_and_parse(std::istringstream& requestStream) {
	std::string line;
	char seperator = ':';
	while (std::getline(requestStream, line, '\n')) {
		std::string key;
		std::string value;
		size_t pos = line.find(seperator);
		if (pos == line.npos)
			break;
		key = line.substr(0, pos);
		value = line.substr(pos + 2, line.length() - (pos + 3));
		this->Headers.insert(std::make_pair(key, value));
	}
}

void HttpRequest::fill_vars_from_headerContainer(void) {
	HeaderContainer::iterator it = this->Headers.find("Host");
	if (it != this->Headers.end())
		this->_Host = it->second;
	it = this->Headers.find("Content-Type");
	if (it != this->Headers.end())
		this->_contentType = it->second;
	it = this->Headers.find("Content-Length");
	if (it != this->Headers.end())
		this->_contentLength = std::atoi((it->second.c_str()));
	it = this->Headers.find("Transfer-Encoding");
	if (it != this->Headers.end() && it->second == "chunked")
		this->isChunked = true;
}

void HttpRequest::parser(std::string request) {
	std::istringstream requestStream(request);
	std::getline(requestStream, this->_RequestLine, '\n');
	size_t pos = this->_RequestLine.find(" ");
	this->_method = this->_RequestLine.substr(0, pos);
	pos++;
	size_t endPos = this->_RequestLine.find(" ", pos);
	this->_path = this->_RequestLine.substr(pos, endPos - pos);
	ckeckForQuery();
	endPos++;
	this->_HttpVersion = this->_RequestLine.substr(endPos, _RequestLine.length() - endPos);
	read_and_parse(requestStream);
	fill_vars_from_headerContainer();
	pos = request.find("boundary");
	if (pos != request.npos) {
		this->bodyExist = true;
		pos = request.find("=", pos);
		endPos = request.find("\n", pos);
		this->_boundary = request.substr(pos + 1, (endPos - pos) - 1);
	}
	pos = request.find("\r\n\r\n");
	if (pos != request.npos) {
		// this->bodyExist = true;
		this->body = request.substr(pos + 4, request.length() - (pos + 4));
		if (!this->body.empty())
			this->bodyExist = true;
	}
	setPortAndServerName();
}

void HttpRequest::setPortAndServerName(void) {
	size_t pos = this->_Host.find(":");
	this->_serverName = this->_Host.substr(0, pos);
	this->_port = std::atoi(this->_Host.substr(pos + 1, this->_Host.length() - (pos + 1)).c_str());
}
std::ostream& operator<<(std::ostream& os, const HttpRequest& obj) {
	os << "####################################################" << std::endl;
	os << "Request Line : " << obj.GetRequestLine() << std::endl;
	os << "method : " << obj.GetMethod() << std::endl;
	os << "path : " << obj.GetPath() << std::endl;
	os << "http version : " << obj.GetHttpVersion() << std::endl;
	os << "HOST : " << obj.GetHost() << std::endl;
	os << "contentType : " << obj.GetContentType() << std::endl;
	os << "contentLength : " << obj.GetContentLength() << std::endl;
	os << "boundary : " << obj.GetBoundary() << std::endl;
	QueryContainer::iterator it = obj.GetQuerty().begin();
	while(it != obj.GetQuerty().end()) {
		os << "Query key : " << it->first << " | value : " << it->second << std::endl;
		it++;
	}
  
	HeaderContainer tmp = obj.GetHeaders();
	HeaderContainer::iterator iter = tmp.begin();
	HeaderContainer::iterator EndIter = tmp.end();
	while (iter != EndIter) {
		os << "Headers key : " << iter->first << " , value : " << iter->second << std::endl;
		iter++;
	}
	os << "body : ---- " << obj.GetBody() << "---- body" << std::endl;
	os << "bodyExist ? : " << obj.bodyExistOrNot() << std::endl;
	os << "chunked ? : " << obj.ChunkedOrNot() << std::endl;
	os << "Port : " << obj.GetPort() << std::endl;
	os << "Server name : " << obj.GetServerName() << std::endl;
	os << "####################################################" << std::endl;
	return os;
}

std::string extructBoundary(std::string requestData, size_t pos) {
	size_t startPos = pos + 9; // 9 is the length of "boundary="
	size_t endPos = requestData.find("\r\n", startPos);
	size_t boundaryLength = endPos - startPos;
	std::string boundary = requestData.substr(startPos, boundaryLength);
	boundary.append("--");
	return boundary;
}

bool requestChecker(std::string requestData) {
	size_t pos = 0;
	if ((pos = requestData.find("\r\n\r\n")) != requestData.npos) {
		if ((pos = requestData.find("boundary")) != requestData.npos) {
			std::string ENDboundary = extructBoundary(requestData, pos);
			if ((pos = requestData.find(ENDboundary)) != requestData.npos)
				return true;
			return false;
		}
		return true;
	}
	return false;
}

