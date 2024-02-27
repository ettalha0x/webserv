/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouchaad <aouchaad@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 22:06:58 by aouchaad          #+#    #+#             */
/*   Updated: 2024/02/27 19:08:16 by aouchaad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../config/configFile.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <sstream>


typedef std::map<std::string, std::string> HeaderContainer;

// typedef std::vector<std::pair<std::string, std::string> > bodyContainer;
// typedef std::vector<std::pair<std::string, int > > fileContainer;
typedef std::vector<std::pair<std::string, std::string> > QueryContainer;

class HttpRequest {
	private:
		std::string 	_RequestLine;  				/*GET /favicon.ico HTTP/1.1*/
		std::string 	_path; 						/*path of the requested resource*/
		std::string 	_method;					/* POST || GET || DELETE */
		std::string 	_HttpVersion;				/*the version of the http protocol being used*/
		std::string 	_Host;
		std::string 	_boundary;
		std::string		_contentType;
		std::string		_requestedFile;
		int				_contentLength;
		int				_port;
		size_t			_bodySize;
		std::string		_serverName;

		HeaderContainer Headers;
		std::string		body;
		QueryContainer  Query;
		
		bool 			bodyExist;					/*there is a body or not */
		bool			isChunked;
		
	public:
		bool			completed;
		bool			served;
		HttpRequest();
		HttpRequest &operator=(const HttpRequest & obj);
		~HttpRequest();

		/*geters*/
		
		void			setPortAndServerName(void);
		
		std::string 	GetRequestLine(void) const;
		std::string 	GetPath(void) const;
		std::string 	GetMethod(void) const;
		std::string 	GetHttpVersion(void) const;
		std::string 	GetHost(void) const;
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
		bool 			bodyExistOrNot(void) const;
		bool 			ChunkedOrNot(void) const;
		
		void 			read_and_parse(std::istringstream &requestStream);
		void 			fill_vars_from_headerContainer(void);
		void 			ckeckForQueryAndRequestedFile(void);
		void 			parser(std::string request);
};

std::ostream& 	operator<<(std::ostream& os, const HttpRequest& obj);
bool 			requestChecker(std::string requestData);
