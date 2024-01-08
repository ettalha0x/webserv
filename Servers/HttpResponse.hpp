# pragma once

#include <iostream>
#include <map>

class HttpResponse {
	private:
		int statusCode; // Default status code
		std::string statusMessage; // Default status message
		std::map<std::string, std::string> headers;
	public:
		HttpResponse();
		// Setters functions
		void	setStatusCode(int statusCode);
		void	setStatusMessage(std::string statusMessage);
		// Function to add a header to the response
		void addHeader(const std::string& key, const std::string& value);

		// Function to get the constructed response header
		std::string getHeaderString() const;

		~HttpResponse();
};