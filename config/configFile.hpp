/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouchaad <aouchaad@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 15:21:02 by aouchaad          #+#    #+#             */
/*   Updated: 2024/01/29 12:11:44 by aouchaad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <sys/stat.h>

typedef struct s_config
{
	int							port;
	std::string 				serverName;
	std::string 				hostName;
	int 						maxBodySize;
	bool 						autoIndex;
	std::string 				rootDir;
	std::vector<std::string	>	indexFile;
	std::string 				cgiPath;
	
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
class DuplicatedPortException : public std::exception {
	public:
		const char * what() const throw() {
			return "the same port used in different servers";
	}
};


std::vector<t_server_config> readConfigeFile(char *path);
void setToDefault(std::vector<t_server_config> &configs);
void printConfigs(std::vector<t_server_config> &configs);
void checkForDuplicatedPorts(std::vector<t_server_config> &configs);