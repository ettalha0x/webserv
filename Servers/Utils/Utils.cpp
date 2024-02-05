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
    std::ifstream file("webserv/Servers/Utils/contentTypes.txt");
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