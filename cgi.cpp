/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esekouni <esekouni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 18:29:12 by esekouni          #+#    #+#             */
/*   Updated: 2024/01/30 18:23:08 by esekouni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>
#include "cgi.hpp"

std::string intToString(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

std::string copy(std::string input, size_t i, size_t j)
{
    if (i >= input.length() || j > input.length() || i > j)
        return "";
    std::string new_str;
    for (size_t index = i; index <= j; ++index)
        new_str.push_back(input[index]);
    return new_str;
}



std::string content_name_value(std::string tmp)
{
	std::string content;
	size_t  i = tmp.find("\"");
	int  j = tmp.find("\n");
	content = copy(tmp, i+1, j-2);
	content += "=";
	tmp = copy(tmp, j+2, tmp.length());
	content += tmp;
	return (content);
}

std::string	 content_type(std::string content, std::string boundry)
{
	size_t i;
	std::string content_type;
	std::string tmp;
	std::string new_boun;
	std::string new_cont;

	size_t j;
	i = 0;
	while (i < boundry.length())
	{
		if (boundry[i] == '\r')
			i++;
		else
		{
			new_boun.push_back(boundry[i]);
			i++;
		}
	}
	new_boun[i] = '\0';
	i = 0;
	while (i < content.length())
	{
		if (content[i] == '\r')
			i++;
		else
		{
			new_cont.push_back(content[i]);
			i++;
		}
	}
	new_cont[i] = '\0';
	i = new_cont.find(new_boun);
	while (1)
	{
		i = new_cont.find(new_boun);
		new_cont = copy(new_cont, i + new_boun.length()+1, new_cont.length());
		j = new_cont.find("name");
		j = new_cont.find(new_boun);
		tmp = copy(new_cont, 1, j-4);
		content_type += content_name_value(tmp);
		new_cont = copy(new_cont, j, new_cont.length());
		i = new_cont.find("name");
		if (i == std::string::npos)
			break;
		else
			content_type += "&";
	}
	// std::cout << "cont   ===   " << content_type ;
	// exit(0);
	return (content_type);
}


cgi::cgi(HttpRequest new_request)
{
	std::cout <<"####################################################" << std::endl;
	std::cout << "Request Line : " << new_request.GetRequestLine() << std::endl;
	std::cout << "method : " << new_request.GetMethod() << std::endl;
	std::cout << "path : " << new_request.GetPath() << std::endl;
	std::cout << "http version : " << new_request.GetHttpVersion() << std::endl;
	std::cout << "HOST : " << new_request.GetHost() << std::endl;
	std::cout << "boundary : " << new_request.GetBoundary() << std::endl;
	std::vector<std::pair<std::string, std::string > > b = new_request.GetQuerty() ;
	std::vector<std::pair<std::string, std::string > >::iterator it_body = b.begin();
	std::cout << "           Querty          "  << std::endl;
	while (it_body != b.end())
	{
		std::cout << it_body->first << " = " << it_body->second << std::endl;
		it_body++;
	}
	std::cout <<"####################################################" << std::endl;

	this->REQUEST_METHOD = new_request.GetMethod();
	this->env.push_back("REQUEST_METHOD=" + new_request.GetMethod());
	this->PATH = new_request.GetPath();
	this->env.push_back("HttpVersion=" + new_request.GetHttpVersion());
	this->env.push_back("SCRIPT_NAME=file.php");
	this->env.push_back("SCRIPT_FILENAME=Sites-available/CGI/file.php");
	this->env.push_back("REDIRECT_STATUS=200");
	// int i;
	// i = this->PATH.rfind('/');
	// this->SCRIPT_NAME = this->PATH.substr(i , this->PATH.length());
	// std::cout << "    HERRRRRRRRR    ==  " << this->SCRIPT_NAME << std::endl;
	// if (new_request.GetBoundary().length() > 0)
	// {
		// this->CONTENT_TYPE = content_type(new_request.GetBody(), new_request.GetBoundary());
		// this->env.push_back("CONTENT_TYPE=" + this->CONTENT_TYPE);
		
	this->body = new_request.GetBody();
	// 	std::cout << "ContentType2 == " <<  this->CONTENT_TYPE << std::endl;
	// 	// exit(0);
	// 	// this->CONTENT_TYPE = new_request.GetContentType();
	// }
	// else
	// {
	// 	std::cout << "bodi =={" << new_request.GetBody()<< "}" << std::endl;
	// 	this->CONTENT_TYPE = new_request.GetBody();
	// 	this->env.push_back("CONTENT_TYPE=" + this->CONTENT_TYPE);
	// 	std::cout << "ContentType1 == " <<  this->CONTENT_TYPE << std::endl;
	// }

	this->env.push_back("CONTENT_TYPE=" + new_request.GetContentType());
	this->env.push_back("CONTENT_LENGTH=" + intToString(new_request.GetContentLength()) );

	if (b.size() > 0)
	{
		it_body = b.begin();
		while (it_body != b.end())
		{
			this->QUERY_STRING += it_body->first + "=" + it_body->second;
			it_body++;
			if (it_body != b.end())
				this->QUERY_STRING += "&";
		}
	}
	if (new_request.GetMethod() == "GET")
		this->env.push_back("QUERY_STRING=" + this->QUERY_STRING);
	else
		this->QUERY_STRING_POST = this->QUERY_STRING;
	
	std::cout << "     CCCCGGGGGGIIIII        "  << this->execute(this->REQUEST_METHOD);
}


char **cgi::convert_to_char_double()
{
	char **envp = new char*[this->env.size() + 1];
	int i = 0;
	std::string tmp;

	std::vector<std::string>::iterator ite = this->env.begin();
	while (ite != this->env.end())
	{
		tmp = *ite;
		char *ptr = new char[tmp.length() + 1];
		std::strcpy(ptr, tmp.c_str());
		ptr[tmp.length()] = '\0';
		envp[i] = ptr;
		i++;
		ite++;
	}
	envp[i] = NULL;
	return envp;
}

std::string	cgi::execute(std::string req_method)
{
	int fd[2];
	std::string result;
	std::string	file_name;
	std::string	type_script;
	char **envp = convert_to_char_double();
	// type_script = "python3";
	file_name = this->PATH;
	if (file_name == "/favicon.ico")
		return ("");
	int i = file_name.find('.');
	std::string ext;
	std::string cmd;
	ext = file_name.substr(i+1, file_name.length());
	if (ext == "py")
	{
		type_script = "python3";
		file_name = "Sites-available/CGI/test.py";
		cmd = "/usr/bin/python3";
	}
	else
	{
		type_script = "./php-cgi";
		file_name = "Sites-available/CGI/file.php";
		cmd = "/Users/esekouni/Desktop/webserv/Sites-available/CGI/php-cgi";
	}
	char *typescript = (char *)type_script.c_str();
	char *filename = (char *)file_name.c_str();
	
	char *args[] = {typescript, filename, NULL};
	int pid;
	int status;
	int is_pipe;

	status = 0;
	is_pipe = 1;
	if (pipe(fd) == -1)
		return ("ERROR\n");
	pid = fork();
	if (pid == -1)
		return ("ERROR\n");
	if (pid == 0)
	{
		if (this->REQUEST_METHOD == "POST")
		{
			if (dup2(fd[0], 0) == -1)
			{
				return ("ERROR\n");
			}
		}
		if (is_pipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close (fd[1]) == -1))
			return ("ERROR\n");
		if (execve(cmd.c_str(), args , envp))
			return ("ERROR\n");
	}
	else
	{
		if (req_method == "POST")
		{
			write(fd[1], this->body.c_str(), this->body.length());
		}
		waitpid(pid, &status, 0); 
		close(fd[1]);
        char buffer[4096];
        ssize_t bytesRead;
        while ((bytesRead = read(fd[0], buffer, sizeof(buffer))) > 0)
			result.append(buffer, bytesRead);
        close(fd[0]);
	}
	
	return (result);
}