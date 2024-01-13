/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouchaad <aouchaad@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 22:06:58 by aouchaad          #+#    #+#             */
/*   Updated: 2024/01/13 18:29:55 by aouchaad         ###   ########.fr       */
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
		HttpRequest();								/*default constructor that initialize method with GET*/
		HttpRequest(std::string method);			/*constructor that initialize with the geven method*/
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
		HeaderContainer GetHeaders(void) const;
		bodyContainer GetBody(void) const;
		int GetStatus(void) const;
		
		void header_parser(std::string request);
		void parser(std::string request);
};

std::ostream& operator<<(std::ostream& os, const HttpRequest& obj);