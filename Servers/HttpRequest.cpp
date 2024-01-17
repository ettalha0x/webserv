/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nettalha <nettalha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 22:07:02 by aouchaad          #+#    #+#             */
/*   Updated: 2024/01/17 17:51:41 by nettalha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest() : bodyExist(false) {}

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

std::string HttpRequest::GetBoundary(void) const {
	return this->_boundary;
}

bodyContainer HttpRequest::GetBody(void) const {
	return this->body;
}

QueryContainer HttpRequest::GetQuerty(void) const {
	return this->Query;
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

void HttpRequest::header_parser(std::string request) {
	size_t pos = request.find("\n");
	this->_RequestLine = request.substr(0, pos);
	pos = this->_RequestLine.find(" ");
	this->_method = this->_RequestLine.substr(0, pos);
	pos++;
	size_t endPos = this->_RequestLine.find(" ", pos);
	this->_path = this->_RequestLine.substr(pos, endPos - pos);
	ckeckForQuery();
	endPos++;
	this->_HttpVersion = this->_RequestLine.substr(endPos, _RequestLine.length() - endPos);
	pos = request.find("Host");
	pos += 6;
	endPos = request.find("\n", pos);
	this->_Host = request.substr(pos, endPos - pos);
	pos = request.find("boundary");
	if (pos != request.npos) {
		this->bodyExist = true;
		pos = request.find("=", pos);
		endPos = request.find("\n", pos);
		this->_boundary = request.substr(pos + 1, (endPos - pos) - 1);
	}
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
	os << "HOST : " << obj.GetHost() << std::endl;
	os << "boundary : " << obj.GetBoundary() << std::endl;
	QueryContainer::iterator it = obj.GetQuerty().begin();
	while(it != obj.GetQuerty().end()) {
		os << "Query key : " << it->first << " | value : " << it->second << std::endl;
		it++;
	}
	os << "####################################################" << std::endl;
	return os;
}

std::string extructBoundary(std::string requestData, size_t pos) {
	size_t startPos = pos + 9; // 9 is the length of "boundary="
	size_t endPos = requestData.find("\n", startPos);
	size_t boundaryLength = endPos - startPos;
	std::string boundary = requestData.substr(startPos, boundaryLength);
	boundary = (boundary + "--");
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