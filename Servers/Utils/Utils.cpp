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
        throw std::runtime_error("Failed to open file: " + fileName);
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return content;
}

int getConfigIndexByPort(int port, const std::vector<t_server_config>& configs) {
    for (int i = 0; i < (int)configs.size(); i++) {
        if (configs[i].port == port) {
            return i;
        }
    }
    return -1;
}

std::string GetExtensionPrefix(std::string extension) {
    std::ifstream file("./Servers/Utils/contentTypes.txt");
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
        exe = filename.substr(i+1, filename.length());
        return (exe);
    }
    else
        return ("");
}