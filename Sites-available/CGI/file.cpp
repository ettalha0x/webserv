#include <stdio.h>
#include <dirent.h>
#include <iostream>

// int main() {  
//     // DIR *dir;
//     // struct dirent *entry;

//     // // Open the current directory
//     // if ((dir = opendir("../Server")) == NULL) {
//     //     perror("opendir");
//     //     return 1;
//     // }

//     // // Read and print entries in the directory
//     // while ((entry = readdir(dir)) != NULL) {
//     //     printf("Name: %s\n", entry->d_name);
//     // }

//     // // Close the directory
//     // if (closedir(dir) == -1) {
//     //     perror("closedir");
//     //     return 1;
//     // }
// 	std::string str = "iuejkbfdn";

// 	int i = str.find('.');
// 	std::cout << i << std::endl;

//     return 0;
// }

#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    // Get environment variables
    const char* scriptFilename = std::getenv("SCRIPT_FILENAME");
    const char* scriptName = std::getenv("SCRIPT_NAME");

    // Get current date and time
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    char timeString[100];
    std::strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTime);

    // Print content type and HTML header
    std::cout << "Content-Type: text/html\r\n\r\n";
    std::cout << "<html><head><title>CGI Example</title></head><body>";

    // Print script information
    std::cout << "<h1>CGI Script Information</h1>";
    std::cout << "<p>SCRIPT_FILENAME: " << (scriptFilename ? scriptFilename : "N/A") << "</p>";
    std::cout << "<p>SCRIPT_NAME: " << (scriptName ? scriptName : "N/A") << "</p>";

    // Print current date and time
    std::cout << "<p>Execution Time: " << timeString << "</p>";

    // Print HTML footer
    std::cout << "</body></html>";

    return 0;
}
