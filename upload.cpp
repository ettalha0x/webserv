#include "upload.hpp"

bool fileExists(const char* filename) {
    std::ifstream file(filename);
    return file.good();
}

void	create_upload_file(std::string body, std::string path)
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
	std::cout << "PATH ====>>>>>    " << path << std::endl;
	if (filename.length() == 0)
		return ;
	if (fileExists(filename.c_str()))
	{
		std::remove(filename.c_str());
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
	
	// i = 

}



void	check_upload(std::string body, std::string path)
{
	size_t i = body.find("Content-Type");
	if (i < body.length())
	{
		create_upload_file(body, path);
	}
	else
		return ;
}


std::vector<std::string>	parse_body(HttpRequest request)
{
	std::string body = request.GetBody();
	std::vector<std::string> vec_body;
	size_t v;
	int len_boudry = request.GetBoundary().length();
	while (1)
	{
		v = body.find(request.GetBoundary());
		if (v < body.length())
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
				// std::cout << "{" << YELLOW << body << RESET << "}" <<  std::endl;
				vec_body.push_back(body.substr(1, body.length()- len_boudry - 9));
				body = body.substr(0, body.length());
			}
		}
		else
			break;
	}
	return (vec_body);
}

void	upload(HttpRequest request, std::string path)
{
	std::string body = request.GetBody();
	std::vector<std::string> vec_body = parse_body(request);
	(void)path;
	std::vector<std::string>::iterator ite = vec_body.begin();
	while (ite != vec_body.end())
	{
		// std::cout << "###################################\n";
		// std::cout << "{" << *ite <<  "}" << std::endl;
		check_upload(*ite, path);
		ite++;
	}
	
	

	
    // if (file.is_open())
	// std::cout << "\n";
}