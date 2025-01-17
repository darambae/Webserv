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
