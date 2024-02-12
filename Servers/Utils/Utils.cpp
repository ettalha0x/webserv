# include "Utils.hpp"


std::string getCurrentTimeInGMT() {
    // Get the current time
    time_t rawTime;
    time(&rawTime);

    // Convert to GMT time
    struct tm* timeInfo;
    timeInfo = gmtime(&rawTime);

    // Format the time as "Sat, 08 Jan 2024 12:00:00 GMT"
    char buffer[80];
    strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S GMT", timeInfo);

    return buffer;
}

 // Include the necessary header file

std::string getFileContent(std::string fileName) {
    // Open the file
    std::ifstream file(fileName);
    // std::string content;
    // if (file.is_open()) {
    //     std::cout << RED << fileName << RESET << std::endl;
    //     std::stringstream fileContent;
    //     fileContent << file.rdbuf();
    //     content = fileContent.str();
    //     file.close();
    // }
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return "";
        // throw std::runtime_error("Failed to open file: " + fileName);
        throw std::runtime_error("Failed to open file: " + fileName);
    }
    // Read the file content
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    // Close the file
    file.close();
    return content;
}

int getConfigIndexByPort(int port, const std::vector<t_server_config>& configs) {
    for (int i = 0; i < (int)configs.size(); i++) {
        if (configs[i].port == port) {
            return i;
        }
    }
    return -1; // Return -1 if no configuration with the given port number is found
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


std::string list_dir(std::string dir_name) {
    DIR *dir = opendir(dir_name.c_str());
    
    if (dir == NULL) {
        return "<h1> Could not open directory: " + dir_name + "</h1>";
    }
    std::string result = "<h1> directory listing for " + dir_name + ": </h1>\n <ul>";
    struct dirent *entry;
    for (entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
        if (entry->d_name[0] != '.') 
            result += std::string("<li> <a href=\"") + entry->d_name + std::string("\"") + std::string(">") + entry->d_name + std::string("</li>") + std::string("\n");
    }
    return result + std::string("</ul>");
};