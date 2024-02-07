# pragma once

# include "../../Webserv.hpp"
# include <iostream>
#include <dirent.h>
# include <vector>
# include <ctime>

std::string getCurrentTimeInGMT();
std::string getFileContent(std::string fileName);
int         getConfigIndexByPort(int port, const std::vector<t_server_config>& configs);
std::string GetExtensionPrefix(std::string extension);
std::string list_dir(std::string dir_name);