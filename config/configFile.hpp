/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouchaad <aouchaad@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 15:21:02 by aouchaad          #+#    #+#             */
/*   Updated: 2024/01/31 22:52:08 by aouchaad         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sys/stat.h>

typedef struct location {
	std::vector<std::string>	paths;
	std::vector<std::string>	indexs;
	std::string 				name;
	std::string 				root;
	bool 						autoIndex;
	int							maxBodySize;
} location;

typedef struct s_config
{
	std::map<std::string, location> locations;
	std::vector<std::string>		indexFile;
	std::string 					serverName;
	unsigned int 					host;
	std::string 					rootDir;
	std::string 					cgiPath;
	bool 							autoIndex;
	int 							maxBodySize;
	int								port;
	
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
class DuplicatedLocationException : public std::exception {
	public:
		const char * what() const throw() {
			return "Duplicated location";
	}
};


std::vector<t_server_config> readConfigeFile(char *path);
void setToDefault(std::vector<t_server_config> &configs);
void printConfigs(std::vector<t_server_config> &configs);
void checkForDuplicatedPorts(std::vector<t_server_config> &configs);