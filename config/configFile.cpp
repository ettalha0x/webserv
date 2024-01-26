/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nettalha <nettalha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 15:21:22 by aouchaad          #+#    #+#             */
/*   Updated: 2024/01/26 19:52:20 by nettalha         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "configFile.hpp"

int lineCount;

void cleanLine(std::string &line) {
	std::string::iterator it = line.begin();
	while (it != line.end()) {
		if (*it == ' ' || *it == '\t')
			it = line.erase(it);
		else
			it++;
	}
}

void checkPath(std::string path) {
	struct stat buffer;
	if (!stat(path.c_str(), &buffer))
		return;
	throw InvalidPathException();
}

std::vector<int> parsePorts(std::string value) {
	// check values and parse theme
	std::vector<int> ports;
	size_t startPos = 0;
	size_t endPos;
	for (size_t i = 0; i < value.length(); i++) {
		if (!std::isdigit(value[i]) && value[i] != ',')
			throw UndefinedValueException();
		if (value[i] == ',' || i == value.length() - 1) {
			endPos = i;
			if (i == value.length() - 1)
				endPos++;
			ports.push_back(std::atoi(value.substr(startPos, endPos - startPos).c_str()));
			startPos = endPos + 1;
		}
	}
	
	
	return ports;
}

void identifieANDfill(std::string line, t_server_config *tmp) {
	size_t seperatorPos = line.find("=");
	size_t delimiterPos = line.find(";");
	if (seperatorPos == line.npos || delimiterPos == line.npos)
		throw SyntaxErrorException();
	std::string key = line.substr(0, seperatorPos);
	std::string value = line.substr(seperatorPos + 1, delimiterPos - (seperatorPos + 1));
	if (key == "serverName") {
		// check errors in value
		tmp->serverName = value;
	} else if (key == "hostName") {
		tmp->hostName = value;
	} else if (key == "maxBodySize") {
		for (size_t i = 0; i < value.length(); i++) {
			if (!std::isdigit(value[i]))
				throw UndefinedValueException();
		}
		tmp->maxBodySize = std::atoi(value.c_str());
	} else if (key == "autoIndex") {
		if (value == "off")
			tmp->autoIndex = false;
		else if (value == "on")
			tmp->autoIndex = true;
		else
			throw UndefinedValueException();
	} else if (key == "rootDir") {
		checkPath(value);
		tmp->rootDir = value;
	} else if (key == "indexFile") {
		checkPath(value);
		tmp->indexFile = value;
	} else if (key == "cgiPath") {
		checkPath(value);
		tmp->cgiPath = value;
	} else if (key == "port") {
		tmp->port = parsePorts(value);
	} else 
		throw UndefinedTokenException();
}

std::vector<t_server_config> readConfigeFile(char *path) {
	std::ifstream configeFile(path);
	if (!configeFile.is_open())
		throw CantOpenConfFileException();
	std::string line;
	bool header;
	bool OpenAccolade;
	bool CloseAccolade;
	std::vector<t_server_config> configs;
	while (1) {
		t_server_config tmp;
		header = OpenAccolade = CloseAccolade = false;
		while (std::getline(configeFile, line, '\n')) {
			lineCount++;
			cleanLine(line);
			if (line.empty())
				continue;
			if (!header) {
				if (line == "server" || line == "server{") {
					header = true;
					if (line == "server{")
						OpenAccolade = true;
					continue;
				}
				else
					throw SyntaxErrorException();
			}
			if (header) {
				if (!OpenAccolade) {
					if (line == "{" || line[0] == '{') {
						OpenAccolade = true;
						if (line == "{")
							continue;
						else
							line.erase(0, 1);
					}
					else
						throw SyntaxErrorException();
				}
				else {
					size_t pos;
					if (line.find('{') != line.npos)
						throw SyntaxErrorException();
					if ((pos = line.find('}')) != line.npos) {
						if (pos == (line.length() - 1)) {
							CloseAccolade = true;
							line.erase(pos, 1);
						}
						else
							throw SyntaxErrorException();
						if (line.empty())
							break;
					}
					if (CloseAccolade && line.find('}') != line.npos)
						throw SyntaxErrorException();
					std::cout << "--" << line << "--" << std::endl;
					identifieANDfill(line, &tmp);
					if (CloseAccolade)
						break;
				}
			}
		}
		if (configeFile.eof()) {
			if (CloseAccolade) {
				configs.push_back(tmp);
				break;
			}
			else
				throw SyntaxErrorException();		
		}
		configs.push_back(tmp);
	}
	configeFile.close();
	return configs;
}

void printConfigs(std::vector<t_server_config> &configs) {
	for (size_t i = 0; i < configs.size(); i++) {
		std::cout << "ports : ";
		for (size_t j = 0; j < configs[i].port.size(); j++) {
			std::cout << configs[i].port[j] << " , ";
		}
		std::cout << std::endl;
		std::cout << "serverName : " << configs[i].serverName << std::endl;
		std::cout << "hostName : " << configs[i].hostName << std::endl;
		std::cout << "maxBodySize : " << configs[i].maxBodySize << std::endl;
		if (configs[i].autoIndex)
			std::cout << "autoIndex : on" << std::endl;
		else
			std::cout << "autoIndex : off" << std::endl;			
		std::cout << "rootDir : " << configs[i].rootDir << std::endl;
		std::cout << "indexFile : " << configs[i].indexFile << std::endl;
		std::cout << "cgiPath : " << configs[i].cgiPath << std::endl;
		std::cout << "**********************************************" << std::endl;
	}
}

void setToDefault(std::vector<t_server_config> &configs) {
	for (size_t i = 0; i < configs.size(); i++) {
		if (configs[i].port.empty())
			configs[i].port.push_back(8080);
		if (configs[i].serverName.empty())
			configs[i].serverName = "localhost";
		if (configs[i].hostName.empty())
			configs[i].hostName = "host";
		if (configs[i].maxBodySize == 0)
			configs[i].maxBodySize = 1024;
		if (configs[i].rootDir.empty())
			configs[i].rootDir = "/Users/aouchaad/Desktop/webserv/Sites-available/Default";
		if (configs[i].indexFile.empty())
			configs[i].indexFile = "/Users/aouchaad/Desktop/webserv/Sites-available/Default";
		if (configs[i].cgiPath.empty())
			configs[i].cgiPath = "/Users/aouchaad/Desktop/webserv/Sites-available/Default";
	}
}