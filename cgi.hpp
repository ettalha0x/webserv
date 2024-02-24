/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nettalha <nettalha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 18:29:15 by esekouni          #+#    #+#             */
/*   Updated: 2024/02/24 15:30:05 by nettalha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <iostream>
#include "Servers/HttpRequest.hpp"

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
		std::string		cgi_res;
		std::string 	finalPath;
		std::string 	cgiPath;
	public:
		// void	set_data_request();
		cgi(HttpRequest new_request, std::string finalPath, std::string cgiPath);
		char **convert_to_char_double();
		std::string execute(std::string req_method, HttpRequest new_request);
		std::string get_cgi_res();
};





#endif