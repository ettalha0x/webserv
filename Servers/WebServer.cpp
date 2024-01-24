# include "WebServer.hpp"
# include "Utils/Utils.hpp"
# include <sys/event.h>

WebServer::WebServer(): Server(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10) {
	launch();
}

void WebServer::accepter() {
	struct sockaddr_in address = get_sock()->get_address();
	int		addrlen = sizeof(address);
	server_socket = accept(get_sock()->get_socket(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
}

void WebServer::handler(int &fdIndex) {
    char buffer[BUFSIZ] = {0};
    int bytesReceived = 0;
    bytesReceived =  recv(client_sockets[fdIndex].fd, buffer, BUFSIZ, 0);
    std::cout << "bytesReceived : " << bytesReceived << std::endl;
    if (stringRequests.find(client_sockets[fdIndex].fd) == stringRequests.end()) {
        // socket not exist yet insert it as a new sokcet
        std::cout << "socket not exist yet insert it as a new sokcet  " << fdIndex << std::endl;
        stringRequests.insert(std::make_pair(client_sockets[fdIndex].fd, buffer));
    } else {
        // socket already exist append it if it's not comleted yet
        std::cout << "socket already exist append it if it's not comleted yet" << std::endl;
        stringRequests[client_sockets[fdIndex].fd].append(buffer);
    }
    HttpRequest newRequest;
	std::cout << "-------------- REQUSTE " << fdIndex << " --------------" << std::endl;
    newRequest.parser(stringRequests[client_sockets[fdIndex].fd]);
    Requests.insert(make_pair(client_sockets[fdIndex].fd, newRequest));
	// std::cout << stringRequests[client_sockets[fdIndex].fd] << std::endl;
}

void WebServer::responder(int &fdIndex) {
    HttpResponse newResponse(Requests[client_sockets[fdIndex].fd]);
    std::string res =  newResponse.getHeader() + newResponse.getBody();
    // int bytesSent = 
    send(client_sockets[fdIndex].fd, res.c_str(), res.length(), 0);
    stringRequests.erase(client_sockets[fdIndex].fd);
    // Requests.erase(client_sockets[fdIndex].fd);
}

void WebServer::launch() {
    int server_socket = get_sock()->get_socket();

    pollfd server_pollfd;
    server_pollfd.fd = server_socket;
    server_pollfd.events = POLLIN | POLLOUT; // Monitor for incoming/outcoming data
    server_pollfd.revents = 0;
    client_sockets.push_back(server_pollfd);  // Add server socket to the list

    int j = 0;
    while (true) {
        if ( j++ % 2 == 0) // this to avoid double printing the message bellow i don't fu** know why but it's working 
            std::cout << "------- WAITING FOR INCOMING REQUESTS -------" << std::endl;


        std::vector<pollfd> tmp = client_sockets;
        int num_events = poll(&tmp[0], tmp.size(), -1);  // Wait indefinitely
        if (num_events == 0) {
            std::cout << "No events occurred" << std::endl;
            continue;  // No events occurred, continue to the next iteration of the loop
        }
        if (num_events == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < (int)tmp.size(); ++i) {
            if (tmp[i].revents & POLLIN) {
                if (tmp[i].fd == server_socket) {
                    accepter();
                    pollfd client_pollfd;
                    client_pollfd.fd = this->server_socket;
                    client_pollfd.events = POLLIN | POLLOUT; // Monitor for incoming/outcoming data
                    client_pollfd.revents = 0;
                    client_sockets.push_back(client_pollfd);  // Add client socket to the list
                } else {
                    handler(i);
                }
            }
            if (tmp[i].revents & POLLOUT) {
                if (requestChecker(stringRequests[tmp[i].fd])){
                    responder(i);
                    close(tmp[i].fd);
                    std::cout << "-------------- DONE --------------" << std::endl;
                    client_sockets.erase(client_sockets.begin() + i);
                }
            }
        }
    }
}

WebServer::~WebServer() {
}
