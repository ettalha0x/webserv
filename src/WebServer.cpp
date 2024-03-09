# include "WebServer.hpp"
# include "Utils/Utils.hpp"
# include <sys/event.h>
#include <cstring>
#include <signal.h>

WebServer::WebServer(std::vector<t_server_config> &configs) : configs(configs) {
    for (size_t i = 0; i < configs.size(); i++) {
		server_listening_sockets.push_back(ListeningSocket(AF_INET, SOCK_STREAM, 0, configs[i].port, INADDR_ANY, 100));
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
    if (!clients[fd].getStringRes().empty())
        return;
    bzero(buffer, BUFFER_SIZE);
    int bytesReceived = 0;
    bytesReceived =  recv(fd, buffer, BUFFER_SIZE, MSG_RCVMORE);
    clients[fd].getPollfd().events |= POLLOUT;

    if (bytesReceived < 0) {
        perror("recv: ");
        return;
    }

    if (bytesReceived > 0) {
        clients[fd].getStringReq().append(buffer, bytesReceived);
    }

    if (bytesReceived > 0 && requestChecker(clients[fd].getStringReq())) {
        clients[fd].resGenerated = false;
        try {
			// std::cout << RED << clients[fd].getStringReq() << std::endl;
            clients[fd].getRequest().parser(clients[fd].getStringReq());
        } catch (...) {
            clients[fd].getRequest().badRequest = true;
        }
        clients[fd].getRequest().completed = true;
        if (!clients[fd].getRequest().badRequest) {
		    HeaderContainer tmp = clients[fd].getRequest().GetHeaders();
		    if (tmp.find("Referer") == tmp.end())
		    {
		    	session ss;
		    	this->ID = ss.create_session(clients[fd].getRequest());
		    }
		    else {
		    	this->ID = tmp["cookie"];
		    }
        }
        clients[fd].getStringReq().clear();
    }
}

bool WebServer::responder(int &fd) {
    if (!clients[fd].resGenerated ){ 
        if (clients[fd].getRequest().badRequest)
            clients[fd].getStringRes() = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 166\r\n\r\n" + std::string(ERROR400) + "\r\n\r\n";
        else if (!checkAllowedChars(clients[fd].getRequest().GetRequestURI())) {
            clients[fd].getStringRes() = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 166\r\n\r\n" + std::string(ERROR400) + "\r\n\r\n";
        }
        else {
            int configIndex = getConfigIndexByPort(clients[fd].getRequest().GetPort(), configs);
            if (configIndex == -1)
                clients[fd].getStringRes() = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 166\r\n\r\n" + std::string(ERROR400) + "\r\n\r\n";
            else {
                HttpResponse newResponse(configs[configIndex], clients[fd].getRequest(), this->ID);
                clients[fd].getStringRes() = newResponse.getHeader() + newResponse.getBody();
                // std::cout << YELLOW << clients[fd].getStringRes() << RESET << std::endl;
                 
            }
        }
        clients[fd].resGenerated = true;
    }
    size_t bytesSent = send(fd, clients[fd].getStringRes().c_str(), clients[fd].getStringRes().length(), 0);
    clients[fd].getPollfd().events |= POLLIN;
    if (bytesSent < 0) {
        perror("write()");
    } else if (bytesSent > 0){
        clients[fd].getStringRes().erase(0, (size_t)bytesSent);
    }
    if (clients[fd].getStringRes().empty()) {
       clients[fd].getStringRes().clear();
       clients[fd].getRequest().served = true;
       if (clients[fd].getRequest().GetHeaders().find("Connection") != clients[fd].getRequest().GetHeaders().end() && clients[fd].getRequest().GetHeaders()["Connection"] == "close")
         {
            close(clients[fd].getPollfd().fd);
            clients.erase(fd);
         } else {
            Client tmp(fd);
            clients[fd] = tmp;
            clients[fd].resGenerated = false;
         }
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
        server_pollfd.events = POLLIN | POLLOUT;
        server_pollfd.revents = 0;
        server_sockets.push_back(server_pollfd);
        clients.insert(std::make_pair(server_pollfd.fd, Client(server_pollfd.fd)));
    }
}

void WebServer::launch() {
    
    init_pollfd();

    while (true) {
        getClientsPollfds();
        int num_events = poll(client_sockets.data(), client_sockets.size(), -1);
        if (num_events == 0) {
            continue;
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
                        break;
                    }
                }

                if (isServerFd) {
                    int new_client = accepter(serverIndex);
                    clients.insert(std::make_pair(new_client, Client(new_client)));
                } else {
                    handler(client_sockets[i].fd);
                }
            }
            if (client_sockets[i].revents & POLLOUT) {
                if (clients[client_sockets[i].fd].getRequest().completed && !clients[client_sockets[i].fd].getRequest().served){
                    if (!responder(client_sockets[i].fd)) {
                        std::cout << GREEN << "Responding..." << RESET << std::endl;
                    }
                }
            }
        }
    }
}

WebServer::~WebServer() {
}