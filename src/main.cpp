/*PSEUDO CODE
1. Open config file
2. Parse -> check validity -> save
3. Initialize
4. Create servers -> run
5. Loop
    5.1 Check clients ()
    5.2
*/

#include "../include/ConfigParser.hpp"
#include "../include/Server.hpp"

int	main(int argc, char **argv) {
	if (argc > 2)
		return 1;
	std::string	configFile;
	if (argc == 2)
		configFile = argv[1];
	ConfigParser	config(configFile);
	Server	server(config.getServers())
}
    5.2 
*/

// #include "../include/ConfigParser.hpp"
// #include "../include/ConfigServer.hpp"
// #include "../include/ConfigLocation.hpp"

// int main(int ac, char **av)
// {
//     if (ac > 2)
//     {
//         std::cerr << "Usage: ./config_parser [config_file]" << std::endl;
//         return 1;
//     }
//     std::string file = "config/default.conf";
//     if (ac == 2)
//         file = av[1];
//     try {
//         ConfigParser parser(file);
//         parser.parseFile();
//     } catch (const char *e) {
//         std::cerr << "Error: " << e << std::endl;
//         return 1;
//     }
//     return 0;
// }
