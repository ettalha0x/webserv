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
    bzero(buffer, 1024);
    int bytesReceived = 0;
    bytesReceived =  recv(client_sockets[fdIndex].fd, buffer, 1024, 0);
    if (bytesReceived < 0) {
        perror("recv: ");
    }
    // buffer[bytesReceived] = '\0'; // Null-terminate the buffer
    if (stringRequests.find(client_sockets[fdIndex].fd) == stringRequests.end()) {
        // socket not exist yet insert it as a new sokcet
        std::cout << "Request not exist yet insert it as a new request  " << fdIndex << std::endl;
        stringRequests.insert(std::make_pair(client_sockets[fdIndex].fd, buffer));
    } else {
            if (bytesReceived > 0) {
                std::cout << "Request already exist append it if it's not comleted yet" << std::endl;
                stringRequests[client_sockets[fdIndex].fd].append(buffer, bytesReceived);
            }
    }
    if (bytesReceived > 0 && requestChecker(stringRequests[client_sockets[fdIndex].fd])) {
        HttpRequest newRequest;
        newRequest.parser(stringRequests[client_sockets[fdIndex].fd]);
        Requests.insert(std::make_pair(client_sockets[fdIndex].fd, newRequest));
        std::cout << Requests[client_sockets[fdIndex].fd] << std::endl;
    }
}

bool WebServer::responder(int &fdIndex) {
    if (stringResponses.find(client_sockets[fdIndex].fd) == stringResponses.end()) {
        int configIndex = getConfigIndexByPort(Requests[client_sockets[fdIndex].fd].GetPort(), configs);
        HttpResponse newResponse(configs[configIndex], Requests[client_sockets[fdIndex].fd]);
        std::string res;
        res = newResponse.getHeader() + newResponse.getBody();
        stringResponses.insert(std::make_pair(client_sockets[fdIndex].fd, res));
    }
    ssize_t bytesSent = write(client_sockets[fdIndex].fd, stringResponses[client_sockets[fdIndex].fd].c_str(), stringResponses[client_sockets[fdIndex].fd].length());
    if (bytesSent > 0) {
        stringResponses[client_sockets[fdIndex].fd].erase(0, (size_t)bytesSent);
        std::cout << GREEN << bytesSent << RESET << std::endl;
    }
    if (stringResponses[client_sockets[fdIndex].fd].empty()) {
       stringResponses.erase(client_sockets[fdIndex].fd);
       return true;
    }
    return false;
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
                    if (responder(i)) {
                        Requests.erase(tmp[i].fd);
                        stringRequests.erase(tmp[i].fd);
                        // close(tmp[i].fd);
                        client_sockets.erase(client_sockets.begin() + i);
                        tmp.erase(tmp.begin() + i);
                    }
                    std::cout << "-------------- DONE --------------" << std::endl;
                }
            }
        }
    }
}

WebServer::~WebServer() {
}
