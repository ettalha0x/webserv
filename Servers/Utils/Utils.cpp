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