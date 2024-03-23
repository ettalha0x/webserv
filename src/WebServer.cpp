# include "WebServer.hpp"
# include "Utils/Utils.hpp"
# include <sys/event.h>
# include <cstring>
# include <signal.h>
# include <algorithm>

std::vector<std::pair<std::string, long> > cookie_vector_expe;

WebServer::WebServer(std::vector<t_server_config> &configs) : configs(configs) {
    std::pair<u_long, int> pair;
    for (size_t i = 0; i < configs.size(); i++) {
        pair.first = configs[i].host;
        for (size_t j = 0; j < configs[i].port.size(); j++) {
            pair.second = configs[i].port[j];
            if (std::find(ipAndPort.begin(), ipAndPort.end(), pair) != ipAndPort.end()) 
                continue;
            ipAndPort.push_back(pair);
            server_listening_sockets.push_back(ListeningSocket(AF_INET, SOCK_STREAM, 0, pair.second, pair.first, SOMAXCONN));
        }
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
    setNonBlocking(new_client_socket);
    if (new_client_socket < 0)
        perror("accept()");
    return new_client_socket;
}

void	WebServer::setNonBlocking(int &sock) {
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0) {
		perror("get flags error");
        close(sock);
    }
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK | O_CLOEXEC)) {
		perror("set to non-blocking error");
        close(sock);
    }
}

void WebServer::handler(int &fd) {
    bzero(buffer, BUFFER_SIZE);
    int bytesReceived = 0;
    bytesReceived =  recv(fd, buffer, BUFFER_SIZE, MSG_RCVMORE);

    if (bytesReceived <= 0) {
            close(clients[fd].getPollfd().fd);
            clients[fd].getStringReq().clear();
            clients.erase(fd);
        return;
    }

    if (bytesReceived > 0) {
        clients[fd].getStringReq().append(buffer, bytesReceived);
    }

    if (bytesReceived > 0 && requestChecker(clients[fd].getStringReq())) {
        try {
            clients[fd].getRequest().parser(clients[fd].getStringReq(), clients[fd].ipAndPort);
        } catch (const BadRequestException &e) {
            clients[fd].getRequest().badRequest = true;
        }
        clients[fd].getRequest().completed = true;
        clients[fd].getStringReq().clear();
    }
}

bool WebServer::responder(int &fd) {
    if (!clients[fd].resGenerated ){
        if (clients[fd].getRequest().badRequest) {
            clients[fd].getStringRes() = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 166\r\n\r\n" + std::string(ERROR400) + "\r\n\r\n";
        }
        else if (!checkAllowedChars(clients[fd].getRequest().GetRequestURI())) {
            clients[fd].getStringRes() = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 166\r\n\r\n" + std::string(ERROR400) + "\r\n\r\n";
        }
        else {
            int configIndex = getMatchedConfig(clients[fd].getRequest(), configs);
            if (configIndex == -1) {
                clients[fd].getStringRes() = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 166\r\n\r\n" + std::string(ERROR400) + "\r\n\r\n";
            }
            else {
                HttpResponse newResponse(configs[configIndex], clients[fd].getRequest(), this->ID);
                clients[fd].getStringRes() = newResponse.getHeader() + newResponse.getBody();
                 
            }
        }
        clients[fd].resGenerated = true;
    }
    size_t bytesSent = send(fd, clients[fd].getStringRes().c_str(), clients[fd].getStringRes().length(), 0);
    if (bytesSent <= 0) {
        close(clients[fd].getPollfd().fd);
        clients[fd].clearData();
        clients.erase(fd);
        return true;
    } else if (bytesSent > 0){
        clients[fd].getStringRes().erase(0, (size_t)bytesSent);
    }
    if (clients[fd].getStringRes().empty()) {

       clients[fd].getRequest().served = true;
            close(clients[fd].getPollfd().fd);
            clients[fd].clearData();
            clients.erase(fd);
            return true;
    }
    return false;
}

void				WebServer::getClientsPollfds() {
    client_sockets.clear();
    for(std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++) {
        bool exist = false;
        for (size_t i = 0; i < client_sockets.size(); i++) {
            if (client_sockets[i].fd == it->second.getPollfd().fd)
            {
                exist = true;
                break;
            }
        }
        if (!exist)
            client_sockets.push_back(it->second.getPollfd());
    }
};

void    WebServer::init_pollfd() {
    for (size_t i = 0; i < server_listening_sockets.size(); i++)
    {
        pollfd server_pollfd;
        server_pollfd.fd = get_server_sock()[i].get_socket();
        server_pollfd.events = POLLIN | POLLOUT;
        server_pollfd.revents = 0;
        server_sockets.push_back(server_pollfd);
        clients.insert(std::make_pair(server_pollfd.fd, Client(server_pollfd.fd, ipAndPort[i])));
    }
}

void	WebServer::check_delete_session()
{
	if (cookie_vector_expe.size() > 0)
	{
		std::vector<std::pair<std::string, long> >::iterator it = cookie_vector_expe.begin();
		std::string path = "session/";
		std::string file_name;
		int i = 0;
		while (it != cookie_vector_expe.end())
		{
			if ((it->second) < std::time(nullptr))
			{
				file_name = path + it->first;
				cookie_vector_expe.erase(cookie_vector_expe.begin() + i);
				std::remove(file_name.c_str());
				it = cookie_vector_expe.begin() + i;
				continue ;
			}
			i++;
			it++;
		}
	}
}

void WebServer::launch() {
    
    init_pollfd();

    while (true) {
        getClientsPollfds();
		check_delete_session();
        int num_events = poll(client_sockets.data(), client_sockets.size(), 0);
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
                    if (new_client > 0) {
                        clients.insert(std::make_pair(new_client, Client(new_client, ipAndPort[serverIndex])));
                    }
                } else {
                    handler(client_sockets[i].fd);
                }
            }
            if (client_sockets[i].revents & POLLOUT) {
                if (clients[client_sockets[i].fd].getRequest().completed && !clients[client_sockets[i].fd].getRequest().served){
                    if (responder(client_sockets[i].fd))
                        client_sockets.erase(client_sockets.begin() + i);
                }
            }
        }
    }
}

WebServer::~WebServer() {
}