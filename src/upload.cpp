#include "upload.hpp"

bool fileExists(const char* filename) {
    std::ifstream file(filename);
	bool var = file.good();
	file.close();
    return var;
}

int	create_upload_file(std::string &body, std::string &path)
{
	std::string filename;
	size_t i = body.find("filename=");
	if (i < body.length())
	{
		i += 10;
		while (body[i] != '"')
		{
			filename.push_back(body[i]);
			i++;
		}
	}
	if (fileExists(filename.c_str()))
	{
		if (std::remove(filename.c_str()) != 0)
			return (500);
	}
	
	i = body.find("Content-Type");
	body = body.substr(i, body.length());
	i = body.find("\n");
	body = body.substr(i + 3, body.length());
	std::ofstream file(path + "/" + filename);
	if (file.is_open())
	{
		file << body;
		file.close();
	}
	else
		return (500);
	return (201);
}



int		check_upload(std::string &body, std::string &path)
{
	// std::cout << "body ==>>{" << body << "}" << std::endl;
	size_t i = body.find("Content-Type");
	size_t j = body.find("\n");
	if (i < body.length() && j < body.length() && body[j-1] == '\r')
	{
		return (create_upload_file(body, path));
	}
	else
		return (201);
}


std::vector<std::string>	parse_body(HttpRequest &request)
{
	std::string body = request.GetBody();
	// std::cout << YELLOW << "body=={" << body << "}" << RESET << std::endl;
	std::vector<std::string> vec_body;
	size_t v;
	int len_boudry = request.GetBoundary().length();
	while (1)
	{
		v = body.find(request.GetBoundary());
		if (v < body.length() && !request.GetBoundary().empty())
		{
			body = body.substr(v + len_boudry, body.length());
			v = body.find(request.GetBoundary());
			if (v < body.length())
			{
				vec_body.push_back(body.substr(1, v-6));
				body = body.substr(v, body.length());
			}
			else
			{
				vec_body.push_back(body.substr(1, body.length()- len_boudry - 9));
				body = body.substr(0, body.length());
			}
		}
		else
			break;
		// std::cout << "parse body" << std::endl;
	}
	return (vec_body);
}

int		upload(HttpRequest &request, std::string &path)
{
	std::string body = request.GetBody();
	std::vector<std::string> vec_body = parse_body(request);
	std::vector<std::string>::iterator ite = vec_body.begin();
	int exitstatus = 201;
	while (ite != vec_body.end())
	{
		exitstatus = check_upload(*ite, path);
		if (exitstatus == 500)
			break ;
		ite++;
	}
	return (exitstatus);
}