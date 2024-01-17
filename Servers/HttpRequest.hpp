/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nettalha <nettalha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 22:06:58 by aouchaad          #+#    #+#             */
/*   Updated: 2024/01/17 17:52:45 by nettalha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <map>
#include <vector>

typedef std::map<std::string, std::string> HeaderContainer;
typedef std::vector<std::pair<std::string, std::string> > bodyContainer;
typedef std::vector<std::pair<std::string, int > > fileContainer;
typedef std::vector<std::pair<std::string, std::string> > QueryContainer;

class HttpRequest {
	private:
		std::string 	_RequestLine;  				/*GET /favicon.ico HTTP/1.1*/
		std::string 	_path; 						/*path of the requested resource*/
		std::string 	_method;					/* POST || GET || DELETE */
		std::string 	_HttpVersion;				/*the version of the http protocol being used*/
		std::string 	_Host;
		std::string 	_boundary;

		HeaderContainer Headers;
		bodyContainer 	body;
		fileContainer   filesUpload;
		QueryContainer  Query;
		
		// bool 			HeaderCompleted;            /* header completed or not */
		bool 			bodyExist;					/*there is a body or not */
		// bool 			bodyCompleted;				/* body completed or not */
		
		int 			Status;
		
	public:
		HttpRequest();
		~HttpRequest();

		void SetRequestLine(std::string RequestLine);
		void SetPath(std::string Path);
		void SetHttpVersion(std::string HttpVersion);
		void SetHost(std::string Host);

		void UpDateStatus(int status);

		// void HeaderIsCompleted(void);
		void BodyDoExist(void);
		// void BodyIsCompleted(void);

		/*geters*/
		
		std::string GetRequestLine(void) const;
		std::string GetPath(void) const;
		std::string GetMethod(void) const;
		std::string GetHttpVersion(void) const;
		std::string GetHost(void) const;
		std::string GetBoundary(void) const;
		HeaderContainer GetHeaders(void) const;
		bodyContainer GetBody(void) const;
		QueryContainer GetQuerty(void) const;
		int GetStatus(void) const;
		
		void header_parser(std::string request);
		void ckeckForQuery(void);
		void parser(std::string request);
};

std::ostream& operator<<(std::ostream& os, const HttpRequest& obj);

std::string extructBoundary(std::string requestData, size_t pos);
bool		requestChecker(std::string requestData);