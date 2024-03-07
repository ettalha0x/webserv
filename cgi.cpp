
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: esekouni <esekouni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 18:29:12 by esekouni          #+#    #+#             */
/*   Updated: 2024/03/06 16:20:59 by esekouni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>
#include "cgi.hpp"
#include <signal.h>

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
    new_str.push_back('\0');
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


cgi::cgi(HttpRequest new_request, std::string finalPath, std::string cgiPath) :finalPath(finalPath), cgiPath(cgiPath)
{
	std::vector<std::pair<std::string, std::string > > b = new_request.GetQuerty() ;
	std::vector<std::pair<std::string, std::string > >::iterator it_Querty = b.begin();
	this->REQUEST_METHOD = new_request.GetMethod();
	this->env.push_back("REQUEST_METHOD=" + new_request.GetMethod());
	this->PATH = new_request.GetPath();
	this->env.push_back("HttpVersion=" + new_request.GetHttpVersion());
	this->env.push_back("SCRIPT_NAME=" + new_request.GetRequestedFile());
	this->env.push_back("SCRIPT_FILENAME=" + finalPath);
	this->env.push_back("REDIRECT_STATUS=200");
	this->body = new_request.GetBody();
	this->env.push_back("CONTENT_TYPE=" + new_request.GetContentType());
	this->env.push_back("CONTENT_LENGTH=" + intToString(new_request.GetContentLength()) );

	if (b.size() > 0)
	{
		it_Querty = b.begin();
		while (it_Querty != b.end())
		{
			this->QUERY_STRING += it_Querty->first + "=" + it_Querty->second;
			it_Querty++;
			if (it_Querty != b.end())
				this->QUERY_STRING += "&";
		}
	}
	if (new_request.GetMethod() == "GET")
		this->env.push_back("QUERY_STRING=" + this->QUERY_STRING);
	else
		this->QUERY_STRING_POST = this->QUERY_STRING;
	this->cgi_res =  this->execute(this->REQUEST_METHOD,new_request);
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

std::pair<std::string , std::string> func(std::string header, std::string key)
{
	std::pair<std::string , std::string> key_value;
	size_t i = header.find(key);
	std::string tmp;
	
	if (i < header.length())
	{
		tmp = header.substr(i, header.length());
		int	x=0;
		size_t k = tmp.find(";");
		if (k < header.length())
		{
			x = 1;
			key_value.first = key;
			key_value.second = copy(tmp, key.length(), k-1);
			// std::cout << "second 1==>> {" << key_value.second << "}" <<  std::endl;
		}
		k = tmp.find("\n");
		if (k < header.length() && x!= 1)
		{
			key_value.first = key;
			key_value.second = copy(tmp, key.length(), k-1);
			// std::cout << "second 2==>> {" << key_value.second << "}" <<  std::endl;
		}
		else if (x!= 1)
		{
			key_value.first = key;
			key_value.second = tmp.substr(key.length(), header.length());
			// std::cout << "second 3==>> {" << key_value.second << "}" <<  std::endl;
		}
	}
	return (key_value);
}

std::map<std::string , std::string> fill_container_map(std::string header)
{
	std::map<std::string , std::string> header_map;
	std::pair<std::string , std::string> key_value;
	
	size_t i = header.find("Content-Length:");
	if (i < header.length())
	{
		key_value = func(header, "Content-Length:");
		header_map[key_value.first] = key_value.second;
	}
	i = header.find("Content-Type:");
	if (i < header.length())
	{
		key_value = func(header, "Content-Type:");
		header_map[key_value.first] = key_value.second;
	}
	return (header_map);
}

std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > cgi::check_resp_cgi(std::string res_cgi, int exitStatus)
{
	std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > resp;
	std::map<std::string , std::string> header_map;


	size_t i = res_cgi.find("\r\n\r\n");
	if (i < res_cgi.length())
	{
		std::string header;
		std::string body;
		int len = res_cgi.length();
		
		header = res_cgi.substr(0, i);
		header_map = fill_container_map(header);
		body = copy(res_cgi, i+4, len);
		// std::cout << "BODY ==={" << body << "}"<<  std::endl;
		size_t j = header.find("Content-Length:");
		double	lent=0;
		if (j < header.length())
		{
			header = copy(header, j + 16, header.length());
			int x =  header.find("\n");
			if (x == -1)
				x = header.length();
			std::string number = copy(header, 0, x-1);
			char *end = NULL;
			lent = std::strtod(number.c_str(), &end);
		}
		// std::cout << "LENT==>>" << lent << std::endl;
		if (lent > 0 && (size_t)lent < body.length())
			resp.second.first = body.substr(0, lent);
		else
		{
			// body.push_back('\0');
			resp.second.first = body.substr(0, body.length() - 2);
			// std::cout << "body==>>" << body.length() << std::endl;
			// std::cout << "body==>>" <<resp.second.first.length() << std::endl;
			// std::cout << YELLOW << "SIZE_BODY ===" << body.length() << RESET << std::endl;
			std::map<std::string , std::string>::iterator it;
			it = header_map.find("Content-Length:");
			if (it != header_map.end())
			{
				std::string len = std::to_string(resp.second.first.length());
				// std::cout << RED << "len ==={" << len << "}"<< RESET <<  std::endl;
				header_map["Content-Length:"] = len;
			}
		}
	}
	else
		resp.second.first = res_cgi;
	if (exitStatus == 0)
		resp.second.second = 200;
	else
		resp.second.second = 500;
	resp.first = header_map;
	return resp;
}

void deleteCharArray(char** arr)
{
    int i = 0;
	while (arr[i])
	{
		delete [] arr[i];
		i++;
	}
    delete [] arr;
}

std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > retern_error(int exitstatus)
{
	std::map<std::string , std::string> map;
	std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > resp;
	resp.first = map;
	resp.second.first = "";
	resp.second.second = exitstatus;
	return (resp);
}


std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > cgi::execute(std::string req_method, HttpRequest new_request)
{
	int fd[2];
	int _fd[2];
	std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > resp;
	std::string result;
	std::string	file_name;
	std::string	type_script;
	int exitStatus = 0;
	char **envp = convert_to_char_double();
	file_name = new_request.GetRequestedFile();
	int i = file_name.find('.');
	std::string ext;
	std::string cmd;
	ext = file_name.substr(i+1, file_name.length());
	if (ext == "py")
	{
		type_script = "python3";
		file_name = finalPath;
		cmd = cgiPath + "/python3";
	}
	else
	{
		type_script = "./php-cgi";
		file_name = finalPath;
		cmd = cgiPath + "/php-cgi";
	}
	char *typescript = (char *)type_script.c_str();
	char *filename = (char *)file_name.c_str();
	
	char *args[] = {typescript, filename, NULL};
	int pid;
	int status;
	int is_pipe;

	status = 0;
	is_pipe = 1;
	if (pipe(fd) == -1 || pipe(_fd) == -1)
	{
		deleteCharArray(envp);
		return (retern_error(500));
	}
	std::time_t currentTime = std::time(nullptr);

	pid = fork();
	if (pid == -1)
	{
		return (retern_error(500));
	}
	if (pid == 0)
	{
		if (this->REQUEST_METHOD == "POST")
		{
			if (dup2(fd[0], 0) == -1 || close(fd[1]) == -1)
			{
				deleteCharArray(envp);
				return (retern_error(500));
			}
		}
		if (close(_fd[0]) == -1|| dup2(_fd[1], 1) == -1)
		{
			deleteCharArray(envp);
			return (retern_error(500));
		}
		if (execve(cmd.c_str(), args , envp) == -1)
		{
			deleteCharArray(envp);
			return (retern_error(500));
		}
	}
	else
	{
		int N = 0;
		time_t time;

		// close(fd[1]);
		// close(_fd[1]);
		// std::cout << "########################1\n";
		close (fd[0]);
		if (req_method == "POST")
		{
			write(fd[1], this->body.c_str(), this->body.length());
		}
		// std::cout << "########################2\n";
		while (1)
		{
			N = waitpid(pid, &status, WNOHANG);
			time = std::time(nullptr);
			if (time > currentTime + 5 && N == 0)
			{
				deleteCharArray(envp);
				kill(pid, SIGKILL);
				return (retern_error(504));
			}
			else if (N != 0)
				break;
		}
		// std::cout << "here" << std::endl;
		if (WIFEXITED(status))
		{
            exitStatus = WEXITSTATUS(status);
			// std::cout << "exitstatus == >>" << exitStatus << std::endl;
		}
        char buffer[4096];
        ssize_t bytesRead;
        close(_fd[1]);
        while ((bytesRead = read(_fd[0], buffer, sizeof(buffer))) > 0)
			result.append(buffer, bytesRead);
        close(fd[1]);
        close(_fd[0]);
	}
	// std::cout << RED << "resut={" << result << "}" << result.length() << RESET << std::endl; 
	deleteCharArray(envp);
	resp = check_resp_cgi(result, exitStatus);
	// std::cout << "SIZE_MAP " << resp.first.size() << std::endl;
	return (resp);
}
std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > cgi::get_cgi_res()
{
	return (this->cgi_res);
}

