# pragma once

# include "Utils/Utils.hpp"
# include "HttpRequest.hpp"
#include <map>

class HttpResponse {
	private:
		HttpRequest 						request;
		int 								statusCode; // Default status code
		std::string 						statusMessage; // Default status message
		std::string 						headerString; // Constructed header string
		std::string 						body; // Body of the response
		t_server_config 					config;
		std::map<std::string, std::string>	headers;

		// Setters function
		void	setStatusCode(int statusCode);
		void	setStatusMessage(std::string statusMessage);


		// Function to get the status code
		int	getStatusCode();
		// Function to get the status message
		std::string	getStatusMessage(int	statusCode);

		// Function to add a header to the response
		void addHeader(const std::string& key, const std::string& value);
		// Function to get the constructed response header
		std::string getHeaderString() const;

		// Function to get the body
		void	constructHeader(void);
		void	constructBody(void);

		
	public:

		HttpResponse(t_server_config &config, HttpRequest &request);
		std::string 	GetFileExtension(std::string path);
		size_t		GetContentLength();
		std::string	getHeader();
		std::string	getBody();

		~HttpResponse();
};