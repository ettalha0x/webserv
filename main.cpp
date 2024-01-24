#include "Webserv.hpp"

int main(){
    t_server_config config;

    std::cout << "---------------- Webserv is starting ----------------" << std::endl;
    config.port.push_back(8080);
    config.rootDir = "Sites-available/Default/";
    config.indexFile = "index.html";
    config.serverName = "webserv.1337.ma";
    config.hostName = "localhost";
    config.maxBodySize = 1024;
    config.autoIndex = false;
    config.cgiPath = "Sites-available/Default/";


    WebServer  webserv(config);
    return 0;
}