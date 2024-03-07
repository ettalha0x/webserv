/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nettalha <nettalha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 18:29:15 by esekouni          #+#    #+#             */
/*   Updated: 2024/03/07 21:26:39 by nettalha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <map>

#include "HttpRequest.hpp"

class cgi
{
	private:
		std::string 	PATH;//y
		std::string		SCRIPT_NAME;
		std::string		SCRIPT_FILENAME;
		std::string		REQUEST_METHOD;//y
		std::string		CONTENT_LENGTH;//y 
		std::string		CONTENT_TYPE;//y
		std::string		QUERY_STRING;//y
		std::string		QUERY_STRING_POST;//y
		std::string		HttpVersion;//y
		std::string		SERVER_SOFTWARE;
		std::string		SERVER_NAME;
		std::string		body;
		std::string		GATEWAY_INTERFACE;
		std::string		REDIRECT_STATUS;
		std::vector< std::string > env;
		std::pair<std::map<std::string , std::string> , std::pair<std::string , int> >	cgi_res;
		std::string 	finalPath;
		std::string 	cgiPath;
	public:
		// void	set_data_request();
		cgi(HttpRequest new_request, std::string finalPath, std::string cgiPath);
		char **convert_to_char_double();
		std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > execute(std::string req_method, HttpRequest new_request);
		std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > get_cgi_res();
		std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > check_resp_cgi(std::string res_body, int exitStatus);
};





#endif