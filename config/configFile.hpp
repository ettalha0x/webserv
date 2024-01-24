/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nettalha <nettalha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 15:21:02 by aouchaad          #+#    #+#             */
/*   Updated: 2024/01/24 15:38:45 by nettalha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <fstream>
#include <vector>

typedef struct s_config
{
	std::vector<int>	port;
	std::string 		serverName;
	std::string 		hostName;
	int 				maxBodySize;
	bool 				autoIndex;
	std::string 		rootDir;
	std::string			indexFile;
	std::string 		cgiPath;
	
} t_server_config;


class SyntaxErrorException : public std::exception {
	public:
		const char * what() const throw() {
			return "Syntax Error";
	}
};
class InvalidPathException : public std::exception {
	public:
		const char * what() const throw() {
			return "Invalid Path";
	}
};
class UndefinedTokenException : public std::exception {
	public:
		const char * what() const throw() {
			return "Undefined Token";
	}
};
class UndefinedValueException : public std::exception {
	public:
		const char * what() const throw() {
			return "Undefined Value";
	}
};
class CantOpenConfFileException : public std::exception {
	public:
		const char * what() const throw() {
			return "cant open confige file";
	}
};


void readConfigeFile(std::string path);