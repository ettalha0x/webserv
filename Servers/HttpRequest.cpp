/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nettalha <nettalha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 22:07:02 by aouchaad          #+#    #+#             */
/*   Updated: 2024/01/17 17:28:49 by nettalha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest() : _method("GET"), HeaderCompleted(false), bodyExist(false), bodyCompleted(false) {}
HttpRequest::HttpRequest(std::string method) : _method(method), HeaderCompleted(false), bodyExist(false), bodyCompleted(false) {}

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

void HttpRequest::HeaderIsCompleted(void) {
	this->HeaderCompleted = true;
}
void HttpRequest::BodyDoExist(void) {
	this->bodyExist = true;
}
void HttpRequest::BodyIsCompleted(void) {
	this->bodyCompleted = true;
}

std::string HttpRequest::GetRequestLine(void) {
	return this->_RequestLine;
}
std::string HttpRequest::GetPath(void) {
	return this->_path;
}
std::string HttpRequest::GetMethod(void) {
	return this->_method;
}
std::string HttpRequest::GetHttpVersion(void) {
	return this->_HttpVersion;
}
std::string HttpRequest::GetHost(void) {
	return this->_Host;
}

HeaderContainer HttpRequest::GetHeaders(void) {
	return this->Headers;
}
bodyContainer HttpRequest::GetBody(void) {
	return this->body;
}
int HttpRequest::GetStatus(void) {
	return this->Status;
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