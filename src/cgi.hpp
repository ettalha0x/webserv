/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esekouni <esekouni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 18:29:15 by esekouni          #+#    #+#             */
/*   Updated: 2024/03/15 20:58:33 by esekouni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <map>
#include "WebServer.hpp"
#include "HttpRequest.hpp"


typedef std::vector<std::pair< std::string, std::string> > vector_cookies;

class cgi
{
	private:
		std::string 	PATH;
		std::string		REQUEST_METHOD;
		std::string		QUERY_STRING;
		std::string		body;
		std::vector< std::string > env;
		std::pair<std::map<std::string , std::string> , std::pair<std::string , int> >	cgi_res;
		std::string 	finalPath;
		std::string 	cgiPath;
		std::vector<std::pair<std::string, std::string> > cookies_experation;
	public:
		cgi(HttpRequest new_request, std::string finalPath, std::string cgiPath);
		char **convert_to_char_double();
		std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > execute(std::string req_method, HttpRequest new_request);
		std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > get_cgi_res();
		std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > check_resp_cgi(std::string res_body, int exitStatus);
};





#endif