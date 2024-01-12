/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouchaad <aouchaad@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 22:07:02 by aouchaad          #+#    #+#             */
/*   Updated: 2024/01/09 23:52:48 by aouchaad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest() : _method("GET"), HeaderCompleted(false), bodyCompleted(false), bodyExist(false) {}
HttpRequest::HttpRequest(std::string method) : _method(method), HeaderCompleted(false), bodyCompleted(false), bodyExist(false){}

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