/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouchaad <aouchaad@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 22:06:58 by aouchaad          #+#    #+#             */
/*   Updated: 2024/03/18 23:44:20 by aouchaad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "configFile.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <sstream>

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define RESET "\033[0m"

typedef std::map<std::string, std::string> HeaderContainer;
typedef std::vector<std::pair<std::string, std::string> > QueryContainer;

class HttpRequest {
	private:
		std::string 	_RequestLine;
		std::string 	_path;
		std::string 	_method;
		std::string 	_HttpVersion;
		u_long		 	_Host;
		std::string 	_boundary;
		std::string		_contentType;
		std::string		_requestedFile;
		std::string		_requestURI;
		int				_contentLength;
		int				_port;
		size_t			_bodySize;
		std::string		_serverName;
		HeaderContainer Headers;
		std::string		body;
		QueryContainer  Query;
		
		bool 			bodyExist;
		bool			isChunked;
		
	public:
		bool			completed;
		bool			served;
		bool			badRequest;
		HttpRequest();
		HttpRequest &operator=(const HttpRequest & obj);
		~HttpRequest();
		
		std::string 	GetRequestLine(void) const;
		std::string 	GetPath(void) const;
		std::string 	GetMethod(void) const;
		std::string 	GetHttpVersion(void) const;
		u_long		 	GetHost(void) const;
		std::string 	GetBoundary(void) const;
		HeaderContainer GetHeaders(void) const;
		std::string 	GetBody(void) const;
		QueryContainer 	GetQuerty(void) const;
		std::string 	GetContentType(void) const;
		std::string		GetServerName(void) const;
		int 			GetContentLength(void) const;
		int				GetPort(void) const;
		size_t 			GetBodySize(void) const;
		std::string		GetRequestedFile(void)const;
		std::string		GetRequestURI(void) const;
		bool 			bodyExistOrNot(void) const;
		bool 			ChunkedOrNot(void) const;
		
		void 			read_and_parse(std::istringstream &requestStream);
		void 			fill_vars_from_headerContainer(void);
		void 			ckeckForQueryAndRequestedFile(void);
		void 			parser(std::string &request, std::pair<u_long, int> ipAndPort);
		void			clearData(void);
};

std::ostream& 	operator<<(std::ostream& os, const HttpRequest& obj);
bool 			requestChecker(std::string &requestData);


class BadRequestException : public std::exception {
	public:
		const char *what() const throw() {
			return "bad request";
		}
};