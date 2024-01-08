# include "WebServer.hpp"
# include "Utils/Utils.hpp"

WebServer::WebServer(): Server(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10) {
	launch();
}

void WebServer::accepter() {
	struct sockaddr_in address = get_sock()->get_address();
	int		addrlen = sizeof(address);
	new_socket = accept(get_sock()->get_socket(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
	request = new char[1024];
	read(new_socket, request, 1024);
}

void WebServer::handler() {
	std::cout << "-------------- REQUSTE RECEVIED --------------" << std::endl;
	std::cout << request << std::endl;
}

void WebServer::responder() {
	static int count = 0;
	response.setStatusCode(200);
	response.setStatusMessage("OK");
	response.addHeader("Content-Type", "text/html");
	response.addHeader("Content-Length", "1337");
    response.addHeader("Server", "Webserv");
    response.addHeader("Date", getCurrentTimeInGMT());
	std::string res = response.getHeaderString();
	write(new_socket, res.c_str(), res.length());
	std::string str = std::to_string(count++);
	write(new_socket, str.c_str(), str.length());
	close(new_socket);
}
void WebServer::launch() {
	while (true) {
		std::cout << "------- WAITING FOR INCOMMING REQUSTES -------" << std::endl;
		accepter();
		handler();
		responder();
		std::cout << "-------------- DONE --------------"<< std::endl;
	}
}

WebServer::~WebServer() {
}

std::string getContentType(const std::string& filePath) {
    // Mapping file extensions to MIME types
    std::map<std::string, std::string> mimeTypes;
    mimeTypes[".html"] = "text/html";
    mimeTypes[".htm"] = "text/html";
    mimeTypes[".txt"] = "text/plain";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "application/javascript";
    // Add more mappings as needed

    // Find the file extension
    size_t dotPosition = filePath.find_last_of(".");
    if (dotPosition != std::string::npos) {
        std::string extension = filePath.substr(dotPosition);

        // Check if the extension is in the map
        std::map<std::string, std::string>::iterator it = mimeTypes.find(extension);
        if (it != mimeTypes.end()) {
            return it->second; // Return the corresponding MIME type
        }
    }

    // Default to application/octet-stream if the extension is not recognized
    return "application/octet-stream";
}

std::streamsize getContentLength(const std::string& filePath) {
    // Open the file in binary mode
    std::ifstream file(filePath.c_str(), std::ios::binary);

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return -1; // Return -1 to indicate an error
    }

    // Get the file size
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.close();

    return fileSize;
}