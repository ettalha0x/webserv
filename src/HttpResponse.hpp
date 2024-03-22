# pragma once

# include "Utils/Utils.hpp"
# include "HttpRequest.hpp"
#include  "cgi.hpp"
#include "upload.hpp"
#include <map>

class HttpResponse {
	private:
		HttpRequest 						request;
		int 								statusCode; // Default status code
		std::string 						statusMessage; // Default status message
		std::string 						headerString; // Constructed header string
		std::string 						body; // Body of the response
		std::string							FinalPath;
		t_server_config 					config;
		std::map<std::string, std::string>	headers;
		std::string 		ID;
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
		location	getMatchedLocation(std::string &locationRoute);

		// Function to get the body
		void	constructHeader(void);
		void	constructBody(void);
		void	check_method(location Location);
		void 	setError(int errorNum, std::string error);
		bool	extentionSuported(std::string, location Location);
		void	callHundlerBymethod(location Location);
		void	PostHundler(location Location);
		void	GetHundler(location Location);
		void	DeleteHundler(location Location);
		void	runCGI(std::string extention, location Location);

		
	public:
		HttpResponse();
		HttpResponse(t_server_config &config, HttpRequest &request, std::string ID);
		std::string 	GetFileExtension(std::string path);
		size_t			GetContentLength();
		std::string		getHeader();
		std::string		getBody();
		void			clearData();

		~HttpResponse();
};

class NotAllowedException : public std::exception {
	public:
		const char * what() const throw() {
			return "Method Not Allowed";
	}
};
class LocationNotFoundException : public std::exception {
	public:
		const char * what() const throw() {
			return "Location Not Found";
	}
};