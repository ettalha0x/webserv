#include "Webserv.hpp"

int main(int ac, char**av){
    
    std::vector<t_server_config> configs;
    signal(SIGPIPE, SIG_IGN);
    if (ac == 2) {
        try {
            configs = readConfigeFile(av[1]);
            setToDefault(configs);
            checkPortsConflicts(configs);
            printConfigs(configs);
        } catch (std::exception &e) {
            std::cout << e.what() << std::endl;
            exit(1);
        } 
    } else {
        t_server_config default_config;
        configs.push_back(default_config);
        setToDefault(configs);
    }

    WebServer webServ(configs);
    return 0;
}