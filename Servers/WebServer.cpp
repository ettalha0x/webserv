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
    bzero(buffer, BUFFER_SIZE);
    int bytesReceived = 0;
    bytesReceived =  recv(client_sockets[fdIndex].fd, buffer, BUFFER_SIZE, MSG_RCVMORE);
    
    // if (bytesReceived == 0) {
    //     std::cout << "Client closed the connection " << std::endl;
    //     close(client_sockets[fdIndex].fd);
    //     client_sockets.erase(client_sockets.begin() + fdIndex);
    //     return;
    // } else 
    if (bytesReceived < 0) {
        std::cout << RED;
        perror("recv: ");
        std::cout << RESET << std::endl;
        return;
    }
    // buffer[bytesReceived] = '\0'; // Null-terminate the buffer
    if (stringRequests.find(client_sockets[fdIndex].fd) == stringRequests.end()) {
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
        newRequest.completed = true;
        if (Requests.find(client_sockets[fdIndex].fd) == Requests.end()) {
            std::cout << "New client " << std::endl;
            Requests.insert(std::make_pair(client_sockets[fdIndex].fd, newRequest));
        }
        else {
            std::cout << "Client already exist " << std::endl;
            Requests[client_sockets[fdIndex].fd] = newRequest;
        }
        client_sockets[fdIndex].events |= POLLOUT;
        stringRequests[client_sockets[fdIndex].fd].clear();
        std::cout << Requests[client_sockets[fdIndex].fd].GetRequestLine() << std::endl;
    }
}

bool WebServer::responder(int &fdIndex) {
    if (!resGenerated){   
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
        resGenerated = true;
    }
    size_t bytesSent = write(client_sockets[fdIndex].fd, stringResponses[client_sockets[fdIndex].fd].c_str(), stringResponses[client_sockets[fdIndex].fd].length());
    if (bytesSent < 0) {
        std::cout << RED;
        perror("write()");
        std::cout << RESET << std::endl;
    } else if (bytesSent > 0){
        stringResponses[client_sockets[fdIndex].fd].erase(0, (size_t)bytesSent);
        std::cout << GREEN << "byte sent: " << bytesSent << RESET << std::endl;
    }
    if (stringResponses[client_sockets[fdIndex].fd].empty()) {
       stringResponses[client_sockets[fdIndex].fd].clear();
       Requests[client_sockets[fdIndex].fd].served = true;
       client_sockets[fdIndex].events |= POLLIN;
       std::cout << GREEN << "-------------- Response sent successfully ! -------------- " << RESET << std::endl;
       resGenerated = false;
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
                        isServerFd = true;
                        serverIndex = j;
                        std::cout << "***** request from server :" << serverIndex << "*****" << std::endl;
                        break;
                    }
                }

                if (isServerFd) {
                    pollfd client_pollfd;
                    client_pollfd.fd = accepter(serverIndex);
                    client_pollfd.events = POLLIN | POLLOUT; // Monitor for incoming/outcoming data
                    client_pollfd.revents = 0;
                    client_sockets.push_back(client_pollfd); // add client to the client_sockets list
                } else {
                    handler(i);
                }
            }
            if (tmp_client_sockets[i].revents & POLLOUT) {
                if (Requests[tmp_client_sockets[i].fd].completed && !Requests[tmp_client_sockets[i].fd].served){
                    if (!responder(i)) {
                        std::cout << GREEN << "-------------- Response NOT completed YET ! -------------- " << RESET << std::endl;
                    }
                }
            }
        }
    }
}

WebServer::~WebServer() {
}
