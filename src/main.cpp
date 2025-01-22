
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
    Logger logger;

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
        std::cout << servers.size() << " servers found" << std::endl;
        printContainer(servers);
    } catch (const std::exception& e) {
        Logger::log(ERROR, "main", e.what(), 0);
        return 1;
    }
    return 0;
}
