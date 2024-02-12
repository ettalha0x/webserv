# include "WebServer.hpp"
# include "Utils/Utils.hpp"
# include <sys/event.h>
#include <cstring>
#include <signal.h>
WebServer::WebServer(std::vector<t_server_config> &configs): Server(configs, AF_INET, SOCK_STREAM, 0, INADDR_ANY, 10), configs(configs) {
	launch();
}

void WebServer::accepter(int    &serverIndex) {
	struct sockaddr_in address = get_server_sock()[serverIndex]->get_address();
	int		addrlen = sizeof(address);
	server_socket = accept(get_server_sock()[serverIndex]->get_socket(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
}

void WebServer::handler(int &fdIndex) {
    // char buffer[1024];
    bzero(buffer, 1024);
    int bytesReceived = 0;
    bytesReceived =  recv(client_sockets[fdIndex].fd, buffer, 1024, 0);
    if (bytesReceived < 0) {
        perror("recv: ");
        exit(EXIT_FAILURE);
    }
    // buffer[bytesReceived] = '\0'; // Null-terminate the buffer
    if (stringRequests.find(client_sockets[fdIndex].fd) == stringRequests.end()) {
        // socket not exist yet insert it as a new sokcet
        std::cout << "Request not exist yet insert it as a new request  " << fdIndex << std::endl;
        // std::string sub(buffer);
        stringRequests.insert(std::make_pair(client_sockets[fdIndex].fd, buffer));
        // std::cout << "****"  << stringRequests[client_sockets[fdIndex].fd] << "****" << std::endl;
    } else {
        // socket already exist append it if it's not comleted yet
        std::cout << "Request already exist append it if it's not comleted yet" << std::endl;
        stringRequests[client_sockets[fdIndex].fd].append(buffer, bytesReceived);
        // std::cout << "####" << stringRequests[client_sockets[fdIndex].fd] << "####" << std::endl;
        // stringRequests[client_sockets[fdIndex].fd] = stringRequests[client_sockets[fdIndex].fd] + std::string(buffer);
        // std::cout << buffer << std::endl;
    }
    if (requestChecker(stringRequests[client_sockets[fdIndex].fd])) {
        HttpRequest newRequest;
        newRequest.parser(stringRequests[client_sockets[fdIndex].fd]);
        Requests.insert(std::make_pair(client_sockets[fdIndex].fd, newRequest));
        std::cout << Requests[client_sockets[fdIndex].fd] << std::endl;
    }
}

void WebServer::responder(int &fdIndex) {
    int configIndex = getConfigIndexByPort(Requests[client_sockets[fdIndex].fd].GetPort(), configs);
    HttpResponse newResponse(configs[configIndex], Requests[client_sockets[fdIndex].fd]);
    std::string res =  newResponse.getHeader() + newResponse.getBody();
    size_t bytesToBeSent = res.length();
    std::cout << GREEN << bytesToBeSent << RESET << std::endl;
    ssize_t bytesSent = 0;
    ssize_t tot = 0;
    while (tot < (ssize_t)bytesToBeSent) {
        bytesSent = send(client_sockets[fdIndex].fd, res.c_str() + tot, bytesToBeSent - tot, 0);
        if (bytesSent < 0) {
            perror("error: ");
            continue;
        }
        std::cout << YELLOW << bytesSent << RESET << std::endl;
        tot += bytesSent;
        std::cout << RED << tot << RESET << std::endl;
    }
    // send(client_sockets[fdIndex].fd, res.c_str(), res.length(), 0);
    // std::cout << "----------------------------------------" << std::endl;
    // std::cout << res << std::endl;
    stringRequests.erase(client_sockets[fdIndex].fd);
    Requests.erase(client_sockets[fdIndex].fd);
}

std::vector<int>    WebServer::init_pollfd() {
    std::vector<int>    server_fds;
    for (size_t i = 0; i < configs.size(); i++)
    {
       int server_fd = get_server_sock()[i]->get_socket();

        pollfd server_pollfd;
        server_pollfd.fd = server_fd;
        server_pollfd.events = POLLIN | POLLOUT; // Monitor for incoming/outcoming data
        server_pollfd.revents = 0;
        client_sockets.push_back(server_pollfd);  // Add server socket to the list
        server_fds.push_back(server_pollfd.fd);
    }
    return server_fds;
}

void WebServer::launch() {
    
   std::vector<int> server_fds = init_pollfd();

    while (true) {
        // std::cout << "------- WAITING FOR INCOMING REQUESTS -------" << std::endl;
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
        for (int i = 0; i < (int)tmp.size(); i++) {
            if (tmp[i].revents & POLLIN) {
                bool isServerFd = false;
                int serverIndex = -1;
                for (int j = 0; j < (int)server_fds.size(); j++) {
                    if (tmp[i].fd == server_fds[j]) {
                        isServerFd = true;
                        serverIndex = j;
                        break;
                    }
                }

                if (isServerFd) {
                    accepter(serverIndex);
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
                    // std::cout << "request received" << std::endl;
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
