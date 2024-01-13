# pragma once

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

# include "Server.hpp"


typedef std::map<int , HttpRequest> RequestContainer;
typedef std::map<int , HttpResponse> ResponseContainer;

class WebServer:  public Server {
	private:
		char	*request;
		int 	new_socket;
		RequestContainer Requests;
		ResponseContainer responses;
		void	accepter();
		void	handler();
		void	responder();

	public:
		WebServer();
		void	launch();
		~WebServer();
};

// Function to get the Content-Type based on file extension
std::string getContentType(const std::string& filePath);

// Function to get the Content-Length based on file size
std::streamsize getContentLength(const std::string& filePath);