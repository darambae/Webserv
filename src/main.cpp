
// #include "../include/ConfigParser.hpp"
// #include "../include/Server.hpp"

// int	main(int argc, char **argv) {
// 	if (argc > 2)
// 		return 1;
// 	std::string	configFile;
// 	if (argc == 2)
// 		configFile = argv[1];
// 	ConfigParser	config(configFile);
// 	Server	server(config.getServers())
// }
#include "../include/webserv.hpp"
#include "../include/ConfigParser.hpp"
#include "../include/ConfigServer.hpp"
#include "../include/ConfigLocation.hpp"


int main(int ac, char **av)
{
    if (ac > 2)
    {
        std::cerr << "Usage: ./config_parser [config_file]" << std::endl;
        return 1;
    }
    std::string file = "config/default.conf";
    if (ac == 2)
        file = av[1];
    try {
        ConfigParser parser(file);
        parser.parseFile();
        const std::vector<ConfigServer>& servers = parser.getServers();
        Logger::getInstance(FILE_OUTPUT).log(INFO, "Configuration file parsed successfully");
        //printContainer(servers);
    } catch (const Exception& e) {
        LOG(e.what());
        return 1;
    }
    return 0;
}
