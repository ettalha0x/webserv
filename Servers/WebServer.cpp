# include "WebServer.hpp"
# include "Utils/Utils.hpp"
# include <sys/event.h>
#include <cstring>
#include <signal.h>

WebServer::WebServer(std::vector<t_server_config> &configs): Server(configs, AF_INET, SOCK_STREAM, 0, INADDR_ANY, 10), configs(configs) {
	launch();
}

int WebServer::accepter(int    &serverIndex) {
	struct sockaddr_in address = get_server_sock()[serverIndex]->get_address();
	int		addrlen = sizeof(address);
	int new_client_socket = accept(get_server_sock()[serverIndex]->get_socket(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_client_socket < 0)
        perror("accept()");
    return new_client_socket;
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
        if (Requests.find(client_sockets[fdIndex].fd) == Requests.end()) {
            Requests.insert(std::make_pair(client_sockets[fdIndex].fd, newRequest));
        }
        else {
            Requests[client_sockets[fdIndex].fd] = newRequest;
        }
        stringRequests[client_sockets[fdIndex].fd].clear();
        std::cout << Requests[client_sockets[fdIndex].fd] << std::endl;
    }
}

bool WebServer::responder(int &fdIndex) {
    int configIndex = getConfigIndexByPort(Requests[client_sockets[fdIndex].fd].GetPort(), configs);
    HttpResponse newResponse(configs[configIndex], Requests[client_sockets[fdIndex].fd]);
    std::string res;
    res = newResponse.getHeader() + newResponse.getBody();
    if (stringResponses.find(client_sockets[fdIndex].fd) == stringResponses.end()) {
        stringResponses.insert(std::make_pair(client_sockets[fdIndex].fd, res));
    }
    else {
        stringResponses[client_sockets[fdIndex].fd] = res;
    }
    ssize_t bytesSent = write(client_sockets[fdIndex].fd, stringResponses[client_sockets[fdIndex].fd].c_str(), stringResponses[client_sockets[fdIndex].fd].length());
    if (bytesSent > 0) {
        stringResponses[client_sockets[fdIndex].fd].erase(0, (size_t)bytesSent);
        std::cout << GREEN << bytesSent << RESET << std::endl;
    }
    if (stringResponses[client_sockets[fdIndex].fd].empty()) {
       stringResponses[client_sockets[fdIndex].fd].clear();
       return true;
    }
    return false;
}

void    WebServer::init_pollfd() {
    for (size_t i = 0; i < configs.size(); i++)
    {
        pollfd server_pollfd;
        server_pollfd.fd = get_server_sock()[i]->get_socket();
        server_pollfd.events = POLLIN | POLLOUT; // Monitor for incoming/outcoming data
        server_pollfd.revents = 0;
        server_sockets.push_back(server_pollfd); // Add server socket to the list
        client_sockets.push_back(server_pollfd); 
    }
}

void WebServer::launch() {
    
    init_pollfd();

    while (true) {
        // std::cout << "------- WAITING FOR INCOMING REQUESTS -------" << std::endl;
        std::vector<pollfd> tmp_client_sockets = client_sockets;
        int num_events = poll(&tmp_client_sockets[0], tmp_client_sockets.size(), -1);  // Wait indefinitely
        if (num_events == 0) {
            std::cout << "No events occurred" << std::endl;
            continue;  // No events occurred, continue to the next iteration of the loop
        }
        if (num_events == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < (int)tmp_client_sockets.size(); i++) {
            if (tmp_client_sockets[i].revents & POLLIN) {
                bool isServerFd = false;
                int serverIndex = -1;
                for (int j = 0; j < (int)server_sockets.size(); j++) {
                    if (tmp_client_sockets[i].fd == server_sockets[j].fd) {
                        tmp_client_sockets.erase(tmp_client_sockets.begin() + i);
                        isServerFd = true;
                        serverIndex = j;
                        break;
                    }
                }

                if (isServerFd) {
                    client_sockets.erase(client_sockets.begin() + i);
                    pollfd client_pollfd;
                    client_pollfd.fd = accepter(serverIndex);
                    client_pollfd.events = POLLIN | POLLOUT; // Monitor for incoming/outcoming data
                    client_pollfd.revents = 0;
                    client_sockets.push_back(client_pollfd); // add client to the client_sockets list
                    tmp_client_sockets.push_back(client_pollfd); 
                } else {
                    handler(i);
                }
            }
            if (tmp_client_sockets[i].revents & POLLOUT) {
                if (requestChecker(stringRequests[tmp_client_sockets[i].fd])){
                    if (responder(i)) {
                        // Requests.erase(tmp_client_sockets[i].fd);
                        // close(tmp_client_sockets[i].fd);
                        // client_sockets.erase(client_sockets.begin() + i);
                        // tmp_client_sockets.erase(tmp_client_sockets.begin() + i);
                    }
                    std::cout << "-------------- DONE --------------" << std::endl;
                }
            }
        }
    }
}

WebServer::~WebServer() {
}
