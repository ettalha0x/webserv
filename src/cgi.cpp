
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

std::string int_to_string(int value)
{
    std::stringstream str;
    str << value;
    return str.str();
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
	this->env.push_back("PATH_INFO=" + finalPath);
	this->env.push_back("REDIRECT_STATUS=200");
	this->body = new_request.GetBody();
	this->env.push_back("CONTENT_TYPE=" + new_request.GetContentType());
	this->env.push_back("CONTENT_LENGTH=" + int_to_string(new_request.GetContentLength()));
	HeaderContainer header_request = new_request.GetHeaders();

	if (!header_request["Cookie"].empty())
	{
		this->env.push_back("HTTP_COOKIE=" + header_request["Cookie"]);
	}
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
		tmp = header.substr(i, header.length()-i);
		int	x=0;
		size_t k = tmp.find(";");
		if (k < header.length() && key != "Set-Cookie:" && key != "Location:")
		{
			x = 1;
			key_value.first = key;
			key_value.second = copy(tmp, key.length()+1, k-1);
		}
		k = tmp.find("\n");
		if (k < header.length() && x!= 1)
		{
			key_value.first = key;
			if (tmp[k-1] == '\r')
				k--;
			key_value.second = copy(tmp, key.length()+1, k-1);
		}
		else if (x!= 1)
		{
			key_value.first = key;
			key_value.second = tmp.substr(key.length()+1, header.length()-key.length()+1);
		}
	}
	return (key_value);
}

std::pair< std::string, std::string> key_value_cookie(std::string tmp)
{
	std::pair< std::string, std::string> key_value;
	size_t i = tmp.find("=");
	if (i < tmp.length())
	{
		key_value.first = tmp.substr(0, i);
		key_value.second = tmp.substr(i+1, tmp.length() - i -1);
	}
	return (key_value);
}

vector_cookies  parse_set_cookie(std::string value)
{
	std::vector<std::pair< std::string, std::string> > vector_pair;
	std::pair< std::string, std::string> key_value;
	std::string tmp;
	size_t i;
	i = value.find(";");
	while (1)
	{
		i = value.find(";");
		if (i == std::string::npos)
			break;
		tmp = value.substr(0, i);
		key_value = key_value_cookie(tmp);
		vector_pair.push_back(key_value);
		value = value.substr(i+2, value.length()-i-2);
	}
	tmp = value.substr(0, tmp.length());
	key_value = key_value_cookie(tmp);
	vector_pair.push_back(key_value);
	
	return (vector_pair);
}


vector_cookies check_attributes_set_cookies(vector_cookies vector_pair, vector_cookies tmp)
{
	if (vector_pair.size())
	{
		for (size_t i = 0; i < tmp.size(); i++)
		{
			for (size_t j = 0; j < vector_pair.size(); j++)
			{
				if (tmp[i].first == vector_pair[j].first)
					break;
				else if (j == vector_pair.size()-1)
				{
					vector_pair.push_back(tmp[i]);
				}
			}
		}
		for (size_t i = 0; i < vector_pair.size(); i++)
		{
			for (size_t j = 0; j < tmp.size(); j++)
			{
				if (vector_pair[i].first == tmp[j].first)
					vector_pair[i].second = tmp[j].second;
			}
		}
	}
	else
	{
		return (tmp);
	}
	return (vector_pair);
}

std::string vector_cookies_to_string(vector_cookies vec)
{
	std::string cookie;
	std::vector<std::pair< std::string, std::string> >::iterator ite ;

	ite = vec.begin();
	while (ite != vec.end())
	{
		cookie += ite->first + "=" + ite->second;
		ite++;
		if (ite != vec.end())
			cookie += "; ";
	}
	return (cookie);
}


std::map<std::string , std::string> fill_container_map(std::string header)
{
	std::map<std::string , std::string> header_map;
	std::pair<std::string , std::string> key_value;
	std::pair<std::string, long> cookie;
	vector_cookies vector_pair;
	vector_cookies tmp_vector_pair;
	
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
	i = header.find("Location:");
	if (i < header.length())
	{
		key_value = func(header, "Location:");
		header_map[key_value.first] = key_value.second;
	}
	i = header.find("Set-Cookie:");
	if (i != std::string::npos)
	{
		while (i < header.length())
		{
			key_value = func(header, "Set-Cookie:");
			tmp_vector_pair = parse_set_cookie(key_value.second);
			vector_pair = check_attributes_set_cookies(vector_pair, tmp_vector_pair);
			header = header.substr(i+12 , header.length()-i-12);
			tmp_vector_pair.clear();
			i = header.find("Set-Cookie:");
		}
		std::string name_seeeion;
		for (size_t i = 0; i < vector_pair.size(); i++)
		{
			if (vector_pair[i].first == "PHPSESSID")
				name_seeeion = "sess_" + vector_pair[i].second;
			if (vector_pair[i].first == "Max-Age")
			{
				cookie.first = name_seeeion;
				cookie.second = std::time(nullptr) + std::atoi(vector_pair[i].second.c_str());
				cookie_vector_expe.push_back(cookie);
			}
				
		}
		
		header_map["Set-Cookie:"] = vector_cookies_to_string(vector_pair);
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

		header = res_cgi.substr(0, i);
		body = copy(res_cgi, header.length()+4, res_cgi.length());
		header_map = fill_container_map(header);
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
		if (lent > 0 && (size_t)lent < body.length())
			resp.second.first = body.substr(0, lent);
		else
		{
			resp.second.first = body.substr(0, body.length() - 2);
			std::map<std::string , std::string>::iterator it;
			it = header_map.find("Content-Length:");
			if (it != header_map.end())
			{
				std::string len = int_to_string(resp.second.first.length());
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
	ext = file_name.substr(i+1, file_name.length()-i-1);
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
		close (fd[0]);
		if (req_method == "POST")
		{
			write(fd[1], this->body.c_str(), this->body.length());
		}
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
		if (WIFEXITED(status))
		{
            exitStatus = WEXITSTATUS(status);
		}
        char buffer[4096];
        ssize_t bytesRead;
        close(_fd[1]);
        while ((bytesRead = read(_fd[0], buffer, sizeof(buffer))) > 0)
			result.append(buffer, bytesRead);
        close(fd[1]);
        close(_fd[0]);
	}
	deleteCharArray(envp);
	resp = check_resp_cgi(result, exitStatus);
	return (resp);
}
std::pair<std::map<std::string , std::string> , std::pair<std::string , int> > cgi::get_cgi_res()
{
	return (this->cgi_res);
}