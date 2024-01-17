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
    buffer = new char[BUFSIZ];
    int bytesReceived = 0;
    bytesReceived =  recv(client_sockets[fdIndex], buffer, BUFSIZ, 0);
    if (stringRequests.find(client_sockets[fdIndex]) == stringRequests.end()) {
        // socket not exist yet insert it as a new sokcet
        std:: cout << "socket not exist yet insert it as a new sokcet" << std::endl;
        stringRequests.insert(std::make_pair(client_sockets[fdIndex], buffer));    
    } else {
        // socket already exist append it if it's not comleted yet
        std:: cout << "socket already exist append it if it's not comleted yet" << std::endl;
        stringRequests[client_sockets[fdIndex]].append(buffer);
    }
    // HttpRequest newRequest;
	std::cout << "-------------- REQUSTE " << fdIndex << " --------------" << std::endl;
    // newRequest.parse(request);
	std::cout << stringRequests[client_sockets[fdIndex]] << std::endl;
    free(buffer);
}

void WebServer::responder(int &fdIndex) {
	static int count = 0;
    HttpResponse newResponse;
	newResponse.setStatusCode(200);
	newResponse.setStatusMessage("OK");
	newResponse.addHeader("Content-Type", "text/html");
	// newResponse.addHeader("Content-Length", "1337");
    newResponse.addHeader("Server", "Wind City Warrior's Web Server");
    newResponse.addHeader("Date", getCurrentTimeInGMT());
	std::string res = newResponse.getHeaderString();
    // int bytesSent = 
    send(client_sockets[fdIndex], res.c_str(), res.length(), 0);
	std::string str = "<center><h1>" + std::to_string(count++) + "</h1></center>\n";
    // int resCounter = 
    send(client_sockets[fdIndex], str.c_str(), str.length(), 0);
    stringRequests.erase(client_sockets[fdIndex]);
	close(client_sockets[fdIndex]);
}

void WebServer::launch() {
    int kq = kqueue();
    if (kq == -1) {
        perror("kqueue");
        exit(EXIT_FAILURE);
    }

    int server_socket = get_sock()->get_socket();
    struct kevent event;
    EV_SET(&event, server_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);

    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
        perror("kevent");
        exit(EXIT_FAILURE);
    }

    std::vector<struct kevent> events(64); // Adjust the size as needed

    int j= 0;
    while (true) {
        if ( j++ % 2 == 0) // this to avoid double printing the message bellow i don't fu** know why but it's working 
            std::cout << "------- WAITING FOR INCOMMING REQUSTES -------" << std::endl;

        int num_events = kevent(kq, NULL, 0, events.data(), events.size(), NULL);
        if (num_events == -1) {
            perror("kevent");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < num_events; ++i) {
            if (static_cast<uintptr_t>(events[i].ident) == static_cast<uintptr_t>(server_socket)) {
                accepter();
                int server_socket = this->server_socket;

                EV_SET(&event, server_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
                    perror("kevent");
                    exit(EXIT_FAILURE);
                }
            } else {
                client_sockets.push_back(events[i].ident);
                handler(i);
                responder(i);
                // Check if the client has closed the connection
                if (events[i].flags & EV_EOF) {
                    close(events[i].ident);
                    std::cout << "Client has closed the connection" << std::endl;
                } else
                    std::cout << "-------------- DONE --------------" << std::endl;
            }
        }
    }

    close(kq);
}

WebServer::~WebServer() {
    delete buffer;
}
