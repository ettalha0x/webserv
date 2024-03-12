/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nettalha <nettalha@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 15:21:22 by aouchaad          #+#    #+#             */
/*   Updated: 2024/03/12 17:47:38 by nettalha         ###   ########.fr       */
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

void checkForDuplicatedPorts(t_server_config configs, int value) {
	for (size_t i = 0 ; i < configs.port.size(); i++) {
		if (configs.port[i] == value)
			throw DuplicatedPortException(); 
	}
}

void checkPortsConflicts(std::vector<t_server_config> &configs) {
	for(size_t i = 0; i < configs.size() - 1; i++) {
		for (size_t j = 0; j < configs[i].port.size(); j++) {
			checkForDuplicatedPorts(configs[i+1], configs[i].port[j]);
		}
	}
}



void checkPath(std::string path) {
	struct stat buffer;
	if (!stat(path.c_str(), &buffer))
		return;
	throw InvalidPathException();
}

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

unsigned long extructHost(std::string value) {
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
	} else if (key == "port") {
		std::vector<std::string> valueParts = parseIndexs(value);
		for (size_t i = 0; i < valueParts.size(); i++) {
			for (size_t index = 0; index < valueParts[i].length(); index++) {
				if (!std::isdigit(valueParts[i][index]))
					throw UndefinedValueException();
			}
			int tmpPort = std::atoi(valueParts[i].c_str());
			checkForDuplicatedPorts(*tmp,tmpPort);
			tmp->port.push_back(tmpPort);
		}
	} else if (key == "ERROR403") {
		checkPath(value);
		tmp->Errors.insert(std::make_pair(403,value));
	} else if (key == "ERROR404") {
		checkPath(value);
		tmp->Errors.insert(std::make_pair(404,value));
	} else if (key == "ERROR405") {
		checkPath(value);
		tmp->Errors.insert(std::make_pair(405,value));
	} else if (key == "ERROR413") {
		checkPath(value);
		tmp->Errors.insert(std::make_pair(413,value));
	} else if (key == "ERROR414") {
		checkPath(value);
		tmp->Errors.insert(std::make_pair(414,value));
	} else if (key == "ERROR500") {
		checkPath(value);
		tmp->Errors.insert(std::make_pair(500,value));
	} else if (key == "ERROR501") {
		checkPath(value);
		tmp->Errors.insert(std::make_pair(501,value));
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
		checkPath(value);
		tmp->root = value;
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
		checkPath(value);
		tmp->redirection = value;
	} else if (key == "upload_path") {
		checkPath(value);
		tmp->upload_path = value;
	} else if (key == "alias") {
		tmp->alias = value;
	} else if (key == "cgi_path") {
		checkPath(value);
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
		// tmp.port = -1;
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
					if ((line.substr(0,8) == "location")) {
						location tmpLocation = locationHandler(line, configeFile);
						if (!checkDuplicatLocation(tmp, tmpLocation.uri))
							tmp.locations.insert(std::make_pair(tmpLocation.uri, tmpLocation));
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
		// std::cout << "rootDir : " << configs[i].rootDir << std::endl;
		for (size_t j = 0; j < configs[i].port.size(); j++)
			std::cout << "port : " << configs[i].port[j] << std::endl;
		// std::cout << "indexFile : " << configs[i].indexFile << std::endl;
			
		// if (configs[i].autoIndex)
		// 	std::cout << "autoIndex : on" << std::endl;
		// else
		// 	std::cout << "autoIndex : off" << std::endl;

		// std::cout << "acceptedMethods : ";	
		// for (size_t j = 0; j < configs[i].acceptedMethods.size(); j++)
		// 	std::cout << configs[i].acceptedMethods[j] << "\t";
		// std::cout << std::endl;
		
		// std::cout << "cgiPath : " << configs[i].cgiPath << std::endl;
		
		// std::cout << "cgi_extentions : ";
		// for (size_t j = 0; j < configs[i].cgi_extentions.size(); j++)
		// 	std::cout << configs[i].cgi_extentions[j] << "\t";
		// std::cout << std::endl;
		
		std::cout << "maxBodySize : " << configs[i].maxBodySize << std::endl;
		
		int number = 1;
		// std::cout << "accepted_extentions : ";
		// for (size_t j = 0; j < configs[i].accepted_extentions.size(); j++)
		// 	std::cout << configs[i].accepted_extentions[j] << "\t";
		// std::cout << std::endl;
		
		for (std::map<std::string, location>:: iterator it = configs[i].locations.begin(); it != configs[i].locations.end(); it++) {
			printLocation(it->second, number);
			number++;
		}
		std::cout << "**********************************************" << std::endl;
	}
}

bool portExist(std::vector<t_server_config> &configs, int port) {
	for (size_t i = 0; i < configs.size(); i++) {
		for (size_t j = 0; j < configs[i].port.size(); j++) {
			if (configs[i].port[j] == port)
			return true;
		}
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

void setDefaultLocation(int i, std::vector<t_server_config> &configs) {
	location tmp;

	tmp.uri = "/";
	tmp.root = "Sites-available/Server_1";
	tmp.cgi_path = "cgi-bin";
	tmp.cgi_extentions.push_back(".php");
	tmp.cgi_extentions.push_back(".py");
	tmp.index = "index.html";
	tmp.autoIndex = false;
	tmp.acceptedMethods.push_back("GET");
	tmp.acceptedMethods.push_back("POST");
	tmp.acceptedMethods.push_back("DELETE");
	configs[i].locations.insert(std::make_pair("/", tmp));
}

void	setLocationsToDefault(t_server_config &config) {
	for (std::map<std::string, location>::iterator it = config.locations.begin(); it != config.locations.end(); it++) {
		if (it->second.root.empty())
			it->second.root = "Sites-available/Server_1";
		if (it->second.cgi_path.empty())
			it->second.cgi_path = "cgi-bin";
		if (it->second.cgi_extentions.empty()) {
			it->second.cgi_extentions.push_back(".php");
			it->second.cgi_extentions.push_back(".py");	
		}
		if (it->second.acceptedMethods.empty()) {
			it->second.acceptedMethods.push_back("GET");
			it->second.acceptedMethods.push_back("POST");
			it->second.acceptedMethods.push_back("DELETE");
		}
		if (it->second.index.empty())
			it->second.index = "index.html";
	}
	
}

void setToDefault(std::vector<t_server_config> &configs) {
	for (size_t i = 0; i < configs.size(); i++) {
		if (configs[i].port.empty())
			configs[i].port.push_back(unusedPort(configs));
		if (configs[i].serverName.empty())
			configs[i].serverName = "localhost";
		if (configs[i].host == 0)
			configs[i].host = 16777343;
		if (configs[i].maxBodySize == 0)
			configs[i].maxBodySize = 1024;
		if (configs[i].locations.find("/") == configs[i].locations.end())
			setDefaultLocation(i, configs);
		setLocationsToDefault(configs[i]);
	}
}