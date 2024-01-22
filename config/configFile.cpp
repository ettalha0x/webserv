/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aouchaad <aouchaad@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 15:21:22 by aouchaad          #+#    #+#             */
/*   Updated: 2024/01/21 19:00:42 by aouchaad         ###   ########.fr       */
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

void readConfigeFile(char *path) {
	std::ifstream configeFile(path);
	if (!configeFile.is_open())
		throw CantOpenConfFileException();
	std::string line;
	bool header = false;
	bool OpenAccolade = false;
	bool CloseAccolade = false;
	// std::vector<t_server_config> configs;
	while (std::getline(configeFile, line, '\n')) {
		lineCount++;
		if (CloseAccolade)
			header = OpenAccolade = CloseAccolade = false;
		// t_server_config tmp;
		if (line.empty())
			continue;
		cleanLine(line);
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
						continue;
				}
				if (CloseAccolade && line.find('}') != line.npos)
					throw SyntaxErrorException();
				std::cout << "--" << line << "--" << std::endl;
			}
		}
		// if (!CloseAccolade && line[line.length() - 1] == '}')
		// 	CloseAccolade = true;
		// configs.push_back(tmp);
	}
	configeFile.close();
}

int main (int ac, char **av) {
	try {
		readConfigeFile(av[1]);
	} catch (std::exception &e) {
		std::cout << "line : " << lineCount << "  error: " << e.what() << std::endl;
	}
}