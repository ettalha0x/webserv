# include "WebServer.hpp"
# include "Utils/Utils.hpp"
# include <sys/event.h>
#include <cstring>
#include <signal.h>

WebServer::WebServer(std::vector<t_server_config> &configs) : configs(configs) {
    for (size_t i = 0; i < configs.size(); i++) {
		server_listening_sockets.push_back(ListeningSocket(AF_INET, SOCK_STREAM, 0, configs[i].port, INADDR_ANY, 10));
	}
    launch();
}

std::vector<ListeningSocket>	WebServer::get_server_sock() {
	return server_listening_sockets;
}

int WebServer::accepter(int    &serverIndex) {
	struct sockaddr_in address = get_server_sock()[serverIndex].get_address();
	int		addrlen = sizeof(address);
	int new_client_socket = accept(get_server_sock()[serverIndex].get_socket(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_client_socket < 0)
        perror("accept()");
    return new_client_socket;
}
 
void WebServer::handler(int &fd) {
    // std::cout << "handling.. " << client_sockets[fdIndex].fd << std::endl;
    if (!clients[fd].getStringRes().empty())
        return;
    bzero(buffer, BUFFER_SIZE);
    int bytesReceived = 0;
    bytesReceived =  recv(fd, buffer, BUFFER_SIZE, MSG_RCVMORE);
    clients[fd].getPollfd().events |= POLLOUT;

    // if (bytesReceived == 0) {
    //     std::cout << RED << "Client closed the connection " << RESET << std::endl;
    //     close(client_sockets[fd].fd);
    //     client_sockets.erase(client_sockets.begin() + fd);
    //     return;
    // } else 
    if (bytesReceived < 0) {
        std::cout << RED;
        perror("recv: ");
        std::cout << RESET << std::endl;
        return;
    }
    // buffer[bytesReceived] = '\0'; // Null-terminate the buffer
    if (bytesReceived > 0) {
        clients[fd].getStringReq().append(buffer, bytesReceived);
    }
    if (bytesReceived > 0 && requestChecker(clients[fd].getStringReq())) {
        clients[fd].resGenerated = false;
        clients[fd].getRequest().parser(clients[fd].getStringReq());
        clients[fd].getRequest().completed = true;
		HeaderContainer tmp = clients[fd].getRequest().GetHeaders();
		if (tmp.find("Referer") == tmp.end())
		{
			session ss;
			this->ID = ss.create_session(clients[fd].getRequest());
			std::cout << this->ID << std::endl;
		}
		else
		{
			this->ID = tmp["cookie"];
		}
        std::cout << RED << clients[fd].getStringReq()<< RESET << std::endl;
        clients[fd].getStringReq().clear();
    }
}

bool WebServer::responder(int &fd) {
    if (!clients[fd].resGenerated ){   
        int configIndex = getConfigIndexByPort(clients[fd].getRequest().GetPort(), configs);
        HttpResponse newResponse(configs[configIndex], clients[fd].getRequest(), this->ID);
        clients[fd].getStringRes() = newResponse.getHeader() + newResponse.getBody();
        clients[fd].resGenerated = true;
    }
    size_t bytesSent = send(fd, clients[fd].getStringRes().c_str(), clients[fd].getStringRes().length(), 0);
    clients[fd].getPollfd().events |= POLLIN;
    if (bytesSent < 0) {
        std::cout << RED;
        perror("write()");
        std::cout << RESET << std::endl;
    } else if (bytesSent > 0){
        clients[fd].getStringRes().erase(0, (size_t)bytesSent);
        std::cout << GREEN << "byte sent: " << bytesSent << RESET << std::endl;
    }
    if (clients[fd].getStringRes().empty()) {
       clients[fd].getStringRes().clear();
       clients[fd].getRequest().served = true;
       if (clients[fd].getRequest().GetHeaders().find("Connection") != clients[fd].getRequest().GetHeaders().end() && clients[fd].getRequest().GetHeaders()["Connection"] == "close")
         {
            std::cout << RED << "Client: " << clients[fd].getPollfd().fd << " Connection closed" << RESET << std::endl;
            close(clients[fd].getPollfd().fd);
            clients.erase(fd);
         } else {
            Client tmp(fd);
            clients[fd] = tmp;
            clients[fd].resGenerated = false;
         }
        std::cout << GREEN << "-------------- Response sent successfully ! -------------- " << RESET << std::endl;
       return true;
    }
    return false;
}

void				WebServer::getClientsPollfds() {
    client_sockets.clear();
    for(std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++) {
        client_sockets.push_back(it->second.getPollfd());
    }
};

void    WebServer::init_pollfd() {
    for (size_t i = 0; i < configs.size(); i++)
    {
        pollfd server_pollfd;
        server_pollfd.fd = get_server_sock()[i].get_socket();
        server_pollfd.events = POLLIN | POLLOUT; // Monitor for incoming/outcoming data
        server_pollfd.revents = 0;
        server_sockets.push_back(server_pollfd); // Add server socket to the list
        // client_sockets.push_back(server_pollfd); 
        clients.insert(std::make_pair(server_pollfd.fd, Client(server_pollfd.fd)));
    }
}

void WebServer::launch() {
    
    init_pollfd();

    while (true) {
        // std::cout << "------- WAITING FOR INCOMING REQUESTS -------" << std::endl;
        getClientsPollfds();
        int num_events = poll(client_sockets.data(), client_sockets.size(), -1);  // Wait indefinitely
        if (num_events == 0) {
            std::cout << "No events occurred" << std::endl;
            continue;  // No events occurred, continue to the next iteration of the loop
        }
        if (num_events == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < (int)client_sockets.size(); i++) {
            if (client_sockets[i].revents & POLLIN) {
                bool isServerFd = false;
                int serverIndex = -1;
                for (int j = 0; j < (int)server_sockets.size(); j++) {
                    if (client_sockets[i].fd == server_sockets[j].fd) {
                        isServerFd = true;
                        serverIndex = j;
                        std::cout << "***** request from server : " << serverIndex << " *****" << std::endl;
                        break;
                    }
                }

                if (isServerFd) {
                    int new_client = accepter(serverIndex);
                    // client_sockets.push_back(client_pollfd); // add client to the client_sockets list
                    // if (new_client > 0)
                    // {
                        clients.insert(std::make_pair(new_client, Client(new_client)));
                        std::cout << "accept new client " << clients[client_sockets[i].fd].getPollfd().fd << std::endl;

                    // }
                } else {
                    handler(client_sockets[i].fd);
                }
            }
            if (client_sockets[i].revents & POLLOUT) {
                if (clients[client_sockets[i].fd].getRequest().completed && !clients[client_sockets[i].fd].getRequest().served){
                    if (!responder(client_sockets[i].fd)) {
                        std::cout << GREEN << "-------------- Response NOT completed YET ! -------------- " << RESET << std::endl;
                    }
                }
            }
        }
    }
}

WebServer::~WebServer() {
}
