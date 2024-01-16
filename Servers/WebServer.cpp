# include "WebServer.hpp"
# include "Utils/Utils.hpp"
# include <sys/event.h>

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
	// read(new_socket, request, BUFSIZ);
    // int bytesReceived = 
    recv(new_socket, request, BUFSIZ, 0);
    HttpRequest newRequest;
	std::cout << "-------------- REQUSTE RECEVIED --------------" << std::endl;
    // newRequest.parse(request);
	std::cout << request << std::endl;
}

void WebServer::responder() {
	static int count = 0;
    HttpResponse newResponse;
	newResponse.setStatusCode(200);
	newResponse.setStatusMessage("OK");
	newResponse.addHeader("Content-Type", "text/html");
	// newResponse.addHeader("Content-Length", "1337");
    newResponse.addHeader("Server", "Webserv");
    newResponse.addHeader("Date", getCurrentTimeInGMT());
	std::string res = newResponse.getHeaderString();
	// write(new_socket, res.c_str(), res.length());
    // int bytesSent = 
    send(new_socket, res.c_str(), res.length(), 0);
	std::string str = std::to_string(count++) + "\n";
	// write(new_socket, str.c_str(), str.length());
    // int resCounter = 
    send(new_socket, str.c_str(), str.length(), 0);
	close(new_socket);
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
                int new_socket = this->new_socket;

                EV_SET(&event, new_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
                    perror("kevent");
                    exit(EXIT_FAILURE);
                }
            } else {
                handler();
                responder();
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
    delete request;
}
