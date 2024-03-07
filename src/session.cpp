#include "session.hpp"
#include <string>

session::session()
{

}

session::~session()
{
	if (this->id_session.size() > 0)
	{
		std::string tmp;
		std::vector<std::string>::iterator it = this->id_session.begin();
		while (it != this->id_session.end())
		{
			tmp = *it;
			std::remove(tmp.c_str());
			it++;
		}
	}
}

std::string Data(std::string ID, HttpRequest new_request)
{ 
    std::string data;   
    data = "UserID = " + ID + "\n";
    if (new_request.GetMethod() == "GET") 
    {
		std::string user="";
		data += user + "\n";
        data += "Path = " + new_request.GetPath() + "\n";
    }
    else
    {
        std::string user="";
        if (new_request.GetBody().length() > 0)
        {
            // int i = new_request.GetBody().find("="); 
            // user = new_request.GetBody().substr(i+1 , new_request.GetBody().length()); 
			data += "Path = " + new_request.GetPath() + "\n";
        } 
        // data += "User_Name = " + user + "\n"; 
    } 
    data += "Current_time = " + getCurrentTimeInGMT(); 
    return (data);

}   
std::string   session::create_session(HttpRequest new_request) 
{ 
    std::map<std::string, std::string> HeaderContainer = new_request.GetHeaders(); 
  
    if (HeaderContainer.find("Cookie") != HeaderContainer.end()) 
    { 
        int i = HeaderContainer["Cookie"].find("="); 
        std::string tmp = "session/" + HeaderContainer["Cookie"].substr(i+1, HeaderContainer["Cookie"].length()); 
        std::string tmp_id = HeaderContainer["Cookie"].substr(i+1, HeaderContainer["Cookie"].length()); 
        std::remove(tmp.c_str()); 
        std::ofstream file(tmp);   
        if (file.is_open()) 
        { 
            // std::cout << "here       new file\n"; 
            file << Data(tmp_id, new_request);
            file.close();
            return (tmp_id);
        }
    }
    else
    {
		std::srand(std::time(nullptr) % 100000);
        int id = std::rand() % 100000 + 1;
        std::string ID = ft_to_string(id);
		this->id_session.push_back(ID);
		// std::cout << 
        std::ofstream outfile("session/" +ID);
        if (outfile.is_open())
        {
            outfile << Data(ID, new_request);
            outfile.close();
            return (ID);
        }
    }
    return ("");
}
