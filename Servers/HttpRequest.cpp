/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouchaad <aouchaad@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 22:07:02 by aouchaad          #+#    #+#             */
/*   Updated: 2024/02/28 16:41:52 by aouchaad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include <unistd.h>

HttpRequest::HttpRequest() : _contentLength(0), _bodySize(0), bodyExist(false), isChunked(false), completed(false), served(false) {}

HttpRequest::~HttpRequest() {}

HttpRequest &HttpRequest::operator=(const HttpRequest & obj) {
	this->_RequestLine = obj._RequestLine;
	this->_path = obj._path;
	this->_method = obj._method;
	this->_HttpVersion = obj._HttpVersion;
	this->_Host = obj._Host;
	this->_boundary = obj._boundary;
	this->_contentType = obj._contentType;
	this->_requestedFile = obj._requestedFile;
	this->_contentLength = obj._contentLength;
	this->_port = obj._port;
	this->_serverName = obj._serverName;
	this->_bodySize = obj._bodySize;
	this->Headers = obj.Headers;
	this->body = obj.body;
	this->Query = obj.Query;
	this->bodyExist = obj.bodyExist;
	this->isChunked = obj.isChunked;
	this->Headers = obj.Headers;
	this->completed = obj.completed;
	this->served = false;
	return *this;
}

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
size_t HttpRequest::GetBodySize(void) const {
	return this->_bodySize;
}
std::string HttpRequest::GetRequestedFile(void) const {
	return _requestedFile;
}
bool HttpRequest::bodyExistOrNot(void) const{
	return this->bodyExist;}
bool HttpRequest::ChunkedOrNot(void) const{
	return this->isChunked;}


void HttpRequest::ckeckForQueryAndRequestedFile(void) {
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
	size_t slashPos = this->_path.find_last_of("/");
	if ((pos = this->_path.find(".")) != this->_path.npos) {
		if (slashPos != _path.npos)
			this->_requestedFile = _path.substr(slashPos + 1, _path.size() - (slashPos + 1));
			_path.erase(slashPos + 1, _path.size() - (slashPos + 1));
	}
	// _path.erase(slashPos + 1, _path.size() - (slashPos + 1));
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
	ckeckForQueryAndRequestedFile();
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
	if (this->bodyExist)
		this->_bodySize = this->body.size();
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
	os << "requestedFile : " << obj.GetRequestedFile() << std::endl;
	os << "boundary : " << obj.GetBoundary() << std::endl;
	
	for (size_t j = 0; j < obj.GetQuerty().size(); j++) {
		os << "Query key : " << obj.GetQuerty()[j].first << " | value : " << obj.GetQuerty()[j].second << std::endl;
	}
  
	HeaderContainer tmp = obj.GetHeaders();
	HeaderContainer::iterator iter = tmp.begin();
	HeaderContainer::iterator EndIter = tmp.end();
	while (iter != EndIter) {
		os << "Headers key : " << iter->first << " , value : " << iter->second << std::endl;
		iter++;
	}
	os << "body : **** " << obj.GetBody() << " **** body" << std::endl;
	os << "bodyExist ? : " << obj.bodyExistOrNot() << std::endl;
	os << "chunked ? : " << obj.ChunkedOrNot() << std::endl;
	os << "Port : " << obj.GetPort() << std::endl;
	os << "Server name : " << obj.GetServerName() << std::endl;
	os << "####################################################" << std::endl;
	return os;
}

bool requestChecker(std::string requestData) {
    size_t pos;
	// std::cout << "+++ " << requestData << " +++" << std::endl;
	// size_t firstNLPos = requestData.find('\n');
    if (requestData.find("GET") == 0) {
        if (requestData.find("\r\n\r\n") != requestData.npos)
            return true;
        return false;
    } else if (requestData.find("POST") == 0) {
        size_t headerEndPos;
        if ((headerEndPos = requestData.find("\r\n\r\n")) != requestData.npos) {
            if (requestData.find("chunked") != requestData.npos) {
                if (requestData.find("\r\n0\r\n") != requestData.npos)
                    return true;
                return false;
            } else if ((pos = requestData.find("Content-Length")) != requestData.npos) {
                size_t endPos = requestData.find("\r\n", pos);
                size_t bodySize = std::atoi((requestData.substr(pos + 16, endPos - (pos + 16))).c_str());
                std::string sub = requestData.substr(headerEndPos + 4, requestData.length() - (headerEndPos + 4));
                if (sub.size() >= bodySize)
                    return true;
                return false;
            }
        }
        return false;
    }
    return false;
}