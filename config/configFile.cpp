/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nettalha <nettalha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 15:21:22 by aouchaad          #+#    #+#             */
/*   Updated: 2024/02/24 15:49:48 by nettalha         ###   ########.fr       */
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

// std::vector<int> parsePorts(std::string value) {
// 	// check values and parse theme
// 	std::vector<int> ports;
// 	size_t startPos = 0;
// 	size_t endPos;
// 	for (size_t i = 0; i < value.length(); i++) {
// 		if (!std::isdigit(value[i]) && value[i] != ',')
// 			throw UndefinedValueException();
// 		if (value[i] == ',' || i == value.length() - 1) {
// 			endPos = i;
// 			if (i == value.length() - 1)
// 				endPos++;
// 			ports.push_back(std::atoi(value.substr(startPos, endPos - startPos).c_str()));
// 			startPos = endPos + 1;
// 		}
// 	}
	
	
// // 	return ports;
// // }
std::vector<std::string> parseIndexs(std::string value) {
	std::vector<std::string> indexs;
	size_t startPos = 0;
	size_t endPos;
	for (size_t i = 0; i < value.length(); i++) {
		if (value[i] == ',' || i == value.length() - 1) {
			endPos = i;
			if (i == value.length() - 1)
				endPos++;
			indexs.push_back(value.substr(startPos, endPos - startPos));
			startPos = endPos + 1;
		}
	}
	
	
	return indexs;
}

std::vector<std::string> mySplite(std::string value) {
	std::vector<std::string> strs;
	for (size_t i = 0; i < value.size(); i++) {
		std::string tmp;
		while (i < value.size() && value[i] != '.')
			tmp.push_back(value[i++]);
		if (tmp.empty())
			throw UndefinedValueException();
		strs.push_back(tmp);
	}
	if (strs.size() != 4)
		throw UndefinedValueException();
	return strs;
}

unsigned int extructHost(std::string value) {
	std::vector<std::string> strs = mySplite(value);
	unsigned int result = 0;
	char *endPtr;
	for (size_t i = 0; i < strs.size(); i++) {
		result += std::strtol(strs[i].c_str(), &endPtr, 10) << i * 8;
		if (endPtr && endPtr[0])
			throw UndefinedValueException();
	}
	return result;
}

void checkforIncorrectMethod(std::vector<std::string> methods) {
	for (size_t i = 0; i < methods.size(); i++) {
		if (methods[i] != "POST" && methods[i] != "DELETE" && methods[i] != "GET")
			throw UndefinedValueException();
	}
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
	} else if (key == "host") {
		tmp->host = extructHost(value);
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
		// checkPath(value);
		tmp->indexFile = value;
	} else if (key == "cgiPath") {
		checkPath(value);
		tmp->cgiPath = value;
	} else if (key == "port") {
		for (size_t index = 0; index < value.length(); index++) {
			if (!std::isdigit(value[index]))
				throw UndefinedValueException();
		}
		tmp->port = std::atoi(value.c_str());
	} else if (key == "acceptedMethods") {
		tmp->acceptedMethods = parseIndexs(value);
		checkforIncorrectMethod(tmp->acceptedMethods);
	} else if (key == "accepted_extentions") {
		tmp->accepted_extentions = parseIndexs(value);
	} else if (key == "cgi_extentions") {
		tmp->cgi_extentions = parseIndexs(value);
	} else 
		throw UndefinedTokenException();
}

void identifieANDfilllocation(std::string line, location *tmp) {
	size_t seperatorPos = line.find("=");
	size_t delimiterPos = line.find(";");
	if (seperatorPos == line.npos || delimiterPos == line.npos)
		throw SyntaxErrorException();
	std::string key = line.substr(0, seperatorPos);
	std::string value = line.substr(seperatorPos + 1, delimiterPos - (seperatorPos + 1));
	if (key == "root") {
		// checkPath(value);
		tmp->root = value;
	// } else if (key == "maxBodySize") {
	// 	for (size_t i = 0; i < value.length(); i++) {
	// 		if (!std::isdigit(value[i]))
	// 			throw UndefinedValueException();
	// 	}
	// 	tmp->maxBodySize = std::atoi(value.c_str());
	} else if (key == "autoIndex") {
		if (value == "off")
			tmp->autoIndex = false;
		else if (value == "on")
			tmp->autoIndex = true;
		else
			throw UndefinedValueException();
	} else if (key == "index") {
		// checkPath(value);
		tmp->index = value;
	} else if (key == "redirection") {
		// check Path
		tmp->redirection = value;
	} else if (key == "upload_path") {
		// check Path
		tmp->upload_path = value;
	} else if (key == "cgi_path") {
		// check Path
		tmp->cgi_path = value;
	} else if (key == "cgi_extentions") {
		tmp->cgi_extentions = parseIndexs(value);
	} else if (key == "acceptedMethods") {
		tmp->acceptedMethods = parseIndexs(value);
		checkforIncorrectMethod(tmp->acceptedMethods);
	}else
		throw UndefinedTokenException();
}

location locationHandler(std::string line, std::ifstream &configFile) {
	location tmp;
	bool start = false;
	bool end = false;
	bool isName = false;
	line.erase(0,8);
	size_t pos;
	if ((pos = line.find("[")) != line.npos) {
		line.erase(pos, 1);
		start = true;
	}
	if (line[0] != '(' || line[line.length() - 1] != ')')
		throw SyntaxErrorException();
	isName = true;
	tmp.uri = line.substr(1,line.length() - 2);
	while (std::getline(configFile, line, '\n')) {
		cleanLine(line);
		if(line.empty())
			continue;
		if (!start) {
			if (line == "[" || line[0] == '[') {
				start = true;
				if (line == "[")
					continue;
				else
					line.erase(0,1);
			}
			else
				throw SyntaxErrorException();
		}
		else {
			size_t pos;
			if (line.find('[') != line.npos)
				throw SyntaxErrorException();
			if ((pos = line.find(']')) != line.npos) {
				if (pos == (line.length() - 1)) {
					end = true;
					line.erase(pos, 1);
				}
				else
					throw SyntaxErrorException();
				if (line.empty())
					break;
			}
			if (end && line.find(']') != line.npos)
				throw SyntaxErrorException();
			identifieANDfilllocation(line, &tmp);
			if (end)
				break;
		}
	}
	return tmp;
}

bool checkDuplicatLocation(t_server_config tmp, std::string name) {
	std::map<std::string, location>::iterator it = tmp.locations.find(name);
	if (it != tmp.locations.end())
		throw DuplicatedLocationException();
	return false;
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
		tmp.host = 0;
		tmp.port = -1;
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
					// std::cout << "--" << line << "--" << std::endl;
					if ((line.substr(0,8) == "location")) {
						location tmpLocation = locationHandler(line, configeFile);
						// std::cout << "%%% " << tmpLocation.maxBodySize << " %%%" << std::endl;
						if (!checkDuplicatLocation(tmp, tmpLocation.uri))
							tmp.locations.insert(std::make_pair(tmpLocation.uri, tmpLocation));
						// std::cout << "^^^^ " << tmp.locations[tmpLocation.name].maxBodySize << " ^^^^^" << std::endl;
					} else
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

void printLocation(location loc, int number) {
	std::cout << "\tlocation " << number << " uri : " << loc.uri << std::endl;
	std::cout << "\tlocation " << number << " root : " << loc.root << std::endl;
	std::cout << "\tlocation " << number << " redirection : " << loc.redirection << std::endl;
	std::cout << "\tlocation " << number << " upload_path : " << loc.upload_path << std::endl;
	std::cout << "\tlocation " << number << " cgi_path : " << loc.cgi_path << std::endl;
	
	std::cout << "\tlocation " << number << " cgi_extention : ";
	for (size_t i = 0; i < loc.cgi_extentions.size(); i++)
		std::cout << loc.cgi_extentions[i] << "\t";
	std::cout << std::endl;
	std::cout << "\tlocation " << number << " index : " << loc.index << std::endl;
	if (loc.autoIndex)
		std::cout << "\tlocation " << number << " autoIndex : on" << std::endl;
	else
		std::cout << "\tlocation " << number << " autoIndex : off" << std::endl;

	std::cout << "\tlocation " << number << " accepted_methods : ";
	for (size_t i = 0; i < loc.acceptedMethods.size(); i++)
		std::cout << loc.acceptedMethods[i] << "\t";
	std::cout << std::endl;
	// std::cout << "\tlocation " << number << " maxBodySize : " << loc.maxBodySize << std::endl;
	std::cout << "----------------------------------------------" << std::endl;
} 

void printConfigs(std::vector<t_server_config> &configs) {
	for (size_t i = 0; i < configs.size(); i++) {
		std::cout << "serverName : " << configs[i].serverName << std::endl;
		std::cout << "host : " << configs[i].host << std::endl;
		std::cout << "rootDir : " << configs[i].rootDir << std::endl;
		std::cout << "port : " << configs[i].port << std::endl;
		std::cout << "indexFile : " << configs[i].indexFile << std::endl;
			
		if (configs[i].autoIndex)
			std::cout << "autoIndex : on" << std::endl;
		else
			std::cout << "autoIndex : off" << std::endl;

		std::cout << "acceptedMethods : ";	
		for (size_t j = 0; j < configs[i].acceptedMethods.size(); j++)
			std::cout << configs[i].acceptedMethods[j] << "\t";
		std::cout << std::endl;
		
		std::cout << "cgiPath : " << configs[i].cgiPath << std::endl;
		
		std::cout << "cgi_extentions : ";
		for (size_t j = 0; j < configs[i].cgi_extentions.size(); j++)
			std::cout << configs[i].cgi_extentions[j] << "\t";
		std::cout << std::endl;
		
		std::cout << "maxBodySize : " << configs[i].maxBodySize << std::endl;
		
		int number = 1;
		std::cout << "accepted_extentions : ";
		for (size_t j = 0; j < configs[i].accepted_extentions.size(); j++)
			std::cout << configs[i].accepted_extentions[j] << "\t";
		std::cout << std::endl;
		
		for (std::map<std::string, location>:: iterator it = configs[i].locations.begin(); it != configs[i].locations.end(); it++) {
			printLocation(it->second, number);
			number++;
		}
		std::cout << "**********************************************" << std::endl;
	}
}

bool portExist(std::vector<t_server_config> &configs, int port) {
	for (size_t i = 0; i < configs.size(); i++) {
		if (configs[i].port == port)
			return true;
	}
	return false;
}

int unusedPort(std::vector<t_server_config> &configs) {
	int port = 8080;
	while (true) {
		if (!portExist(configs, port))
			break;
		port++;
	}
	return port;
}

void checkForDuplicatedPorts(std::vector<t_server_config> &configs) {
	for (size_t i = 0; i < configs.size(); i++) {
		for (size_t j = i + 1; j < configs.size(); j++) {
			if (configs[i].port == configs[j].port)
				throw DuplicatedPortException(); 
		}
	}
}

void setDefaultLocation(int i, std::vector<t_server_config> &configs) {
	location tmp;

	tmp.acceptedMethods = configs[i].acceptedMethods;
	tmp.autoIndex = configs[i].autoIndex;
	tmp.cgi_extentions = configs[i].cgi_extentions;
	tmp.cgi_path = configs[i].cgiPath;
	tmp.index = configs[i].indexFile;
	tmp.root = configs[i].rootDir;
	configs[i].locations.insert(std::make_pair("/", tmp));
}

void setToDefault(std::vector<t_server_config> &configs) {
	for (size_t i = 0; i < configs.size(); i++) {
		if (configs[i].port == -1)
			configs[i].port = unusedPort(configs);
		if (configs[i].serverName.empty())
			configs[i].serverName = "localhost";
		if (configs[i].host == 0)
			configs[i].host = 16777343;
		if (configs[i].maxBodySize == 0)
			configs[i].maxBodySize = 1024;
		if (configs[i].rootDir.empty())
			configs[i].rootDir = "/Users/aouchaad/Desktop/webserv/Sites-available/Server_1";
		// if (configs[i].indexFile.empty())
		// 	configs[i].indexFile.push_back("index.html");
		// if (configs[i].cgiPath.empty())
		// 	configs[i].cgiPath = "/Users/aouchaad/Desktop/webserv/Sites-available/CGI";
		if (configs[i].cgi_extentions.empty()) {
			configs[i].cgi_extentions.push_back(".py");
			configs[i].cgi_extentions.push_back(".php");
		}
		if (configs[i].locations.find("/") == configs[i].locations.end())
			setDefaultLocation(i, configs);	
	}
}