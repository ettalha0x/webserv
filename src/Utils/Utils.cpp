# include "Utils.hpp"


std::string getCurrentTimeInGMT() {
    time_t rawTime;
    time(&rawTime);

    struct tm* timeInfo;
    timeInfo = gmtime(&rawTime);

    char buffer[80];
    strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", timeInfo);

    return buffer;
}


std::string getFileContent(std::string fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return content;
}

int getMatchedConfig(HttpRequest request, const std::vector<t_server_config> configs)
{
    std::vector<int> newConfig;
    for (size_t i = 0; i < configs.size(); i++)
    {
        // Check if host and port match
        if (configs[i].host == request.GetHost() && std::find(configs[i].port.begin(), configs[i].port.end(), request.GetPort()) != configs[i].port.end())
        {
            newConfig.push_back(i);
        }
    }
    for (size_t i = 0; i < newConfig.size(); i++)
    {
        if (request.GetServerName() == configs[newConfig[i]].serverName)
            return newConfig[i];
    }

    return newConfig[0];
}


std::string GetExtensionPrefix(std::string extension) {
    std::ifstream file("./src/Utils/contentTypes.txt");
    if (!file)
        return "text/" + extension;
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find("/" + extension);
        if (pos != line.npos) {
            file.close();
            return line;
        }
    }
    file.close();
    return "text/" + extension;
}

bool is_directory(const std::string& path) {
  struct stat statbuf;
  if (stat(path.c_str(), &statbuf) == -1) {
    return false;
  }
  return S_ISDIR(statbuf.st_mode);
}

std::string list_dir(std::string dir_name) {
    DIR *dir = opendir(dir_name.c_str());
    
    if (dir == NULL) {
        return "<h1> Could not open directory: " + dir_name + "</h1>";
    }
    std::string result = "<!DOCTYPE html><html><meta charset=\"UTF-8\"><body><center><h1> Directory listing: </h1></center><hr><ul>";
    struct dirent *entry;
    for (entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
        if (entry->d_name[0] != '.') {
            if (entry->d_type == DT_DIR) {
                result += std::string("<li> 📁 <a href=\"") + entry->d_name + "/\">" + entry->d_name + "</a></li>\n";
            }
            else {
                result += std::string("<li> 📄 <a href=\"") + entry->d_name + "\">" + entry->d_name + "</a></li>\n";
            }
        }
    }
    closedir(dir);
    return result + "</ul></body></html>";
}

bool    alreadyExist(const std::vector<pollfd>& vector, int element) {
  for (size_t i = 0; i < vector.size(); ++i) {
    if (vector[i].fd == element) {
      return true;
    }
  }
  return false;
}

std::string getCgiExtension(std::string filename) {
    size_t i = filename.find_last_of(".");
    if (i < filename.length())
    {
        std::string exe;
        exe = filename.substr(i, filename.length());
        return (exe);
    }
    else
        return ("");
}

bool checkAllowedChars(const std::string uri) {
    static const char allowedCharsArray[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '.', '_',
        '~', ':', '/', '?', '#', '[', ']', '@', '!', '$', '&', '\'', '(',
        ')', '*', '+', ',', ';', '='
    };

    static const std::set<char> allowedChars(allowedCharsArray, allowedCharsArray + (sizeof(allowedCharsArray) / sizeof(allowedCharsArray[0]))); // add allowed chars to set by array with size

    for (std::string::const_iterator it = uri.begin(); it != uri.end(); ++it) {
        if (allowedChars.find(*it) == allowedChars.end()) {
            return false;
        }
    }
    return true;
} 