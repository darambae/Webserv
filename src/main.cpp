
#include "../include/webserv.hpp"

int MAX_CLIENT = 1024;

std::map<int, Fd_data*>	FD_DATA;
std::vector<struct pollfd> ALL_FDS;

volatile sig_atomic_t stopProgram = 0;

void    signalHandler(int signal) {
    LOG_DEBUG("Interrupt signal " + to_string(signal) + " received");
    stopProgram = 1;
    for(std::map<int, Fd_data*>::iterator it = FD_DATA.begin(); it != FD_DATA.end(); ++it) {
        CgiManager* cgi = it->second->CGI;
        if (cgi) {
            kill(cgi->getPid(), SIGTERM);
            delete cgi;
        }
    }

	if (FD_DATA.size() > 0) {
		for (std::map<int, Fd_data*>::iterator it = FD_DATA.begin(); it != FD_DATA.end(); ++it) {
			close(it->first);
			if (it->second->status == CLIENT)
				delete it->second->request;
			delete it->second;
		}
		FD_DATA.clear();
	}
	if (ALL_FDS.size() > 0)
		ALL_FDS.clear();
}

int main(int ac, char **av)
{
    signal(SIGPIPE, SIG_IGN);
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signalHandler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    if (ac > 2)
    {
        LOG_ERROR("Usage: ./config_parser config/[config_file]", 0);
        return 1;
    }
    std::string file = ac == 2 ? av[1] : "config/default.conf";
    try {
        ConfigParser parser(file);
        parser.parseFile();
        const std::vector<ConfigServer>& servers = parser.getServers();
        LOG_INFO("Configuration file parsed successfully");
        ServerManager	manager(servers);
		manager.launchServers();

    } catch (std::exception & e) {
        LOG_ERROR(e.what(), 0);
    }
    return 0;
}
