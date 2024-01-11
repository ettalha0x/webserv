# include "WebServer.hpp"
# include "Utils/Utils.hpp"

WebServer::WebServer(): Server(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10) {
	launch();
}

void WebServer::accepter() {
	struct sockaddr_in address = get_sock()->get_address();
	int		addrlen = sizeof(address);
	new_socket = accept(get_sock()->get_socket(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
}

void WebServer::handler() {
    request = new char[BUFSIZ];
	read(new_socket, request, BUFSIZ);
	std::cout << "-------------- REQUSTE RECEVIED --------------" << std::endl;
	std::cout << request << std::endl;
}

void WebServer::responder() {
	static int count = 0;
	response.setStatusCode(200);
	response.setStatusMessage("OK");
	response.addHeader("Content-Type", "text/html");
	// response.addHeader("Content-Length", "1337");
    response.addHeader("Server", "Webserv");
    response.addHeader("Date", getCurrentTimeInGMT());
	std::string res = response.getHeaderString();
	write(new_socket, res.c_str(), res.length());
	std::string str = std::to_string(count++) + "\n";
	write(new_socket, str.c_str(), str.length());
	close(new_socket);
}

void WebServer::launch() {
    fd_set  curr_socket, ready_socket;
    int max_sockfd = get_sock()->get_socket();

    // init current socket
    FD_ZERO(&curr_socket);
    FD_SET(get_sock()->get_socket(), &curr_socket);
    int j = 0;
	while (true) {
        //save the current socket because select() is destructive
        ready_socket = curr_socket;

        if ( j++ % 2 == 0) // this to avoid double printing the message bellow i don't fu** know why but it's working 
            std::cout << "------- WAITING FOR INCOMMING REQUSTES -------" << std::endl;
        if (select(max_sockfd + 1, &ready_socket, NULL, NULL, NULL) < 0) {
            perror("select()");
            exit(EXIT_FAILURE);
        }

        for (int fd=0; fd < (max_sockfd + 1); fd++) {
            if (FD_ISSET(fd, &ready_socket)) {
                if (fd == get_sock()->get_socket()) {
                    accepter();
                    FD_SET(new_socket, &curr_socket);
                    if (new_socket > max_sockfd) {
                        max_sockfd = new_socket;
                    }
                } else {
                    handler();
                    responder();
                    FD_CLR(fd, &curr_socket);
		            std::cout << "-------------- DONE --------------"<< std::endl;
                }
            }
        }
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