#pragma once

#include "webserv.hpp"

class	CgiManager {
	private:
		std::string		_path;
		std::vector<std::string>	_argv;
		std::vector<std::string>	_envp;

	public:
		CgiManager(std::string	path_cgi) : _path(path_cgi) {

		}
		int	forkProcess() {
			int	sockets_cgi[2];
			if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK |SOCK_CLOEXEC, 0, sockets_cgi) == -1) {
				LOG_ERROR("socketpair failed", true);
				return -1;
			}
			pid_t	pid = fork();
			if (pid == -1) {
				LOG_ERROR("fork failed", true);
				return -1;
			}
			if (pid == 0) {
				close(sockets_cgi[0]);//will be use by parent
				dup2(sockets_cgi[1], STDIN_FILENO);
				dup2(sockets_cgi[1], STDOUT_FILENO);
				close(sockets_cgi[1]);
				execve(_path.c_str(), convertVectorToChartab(_argv), convertVectorToChartab(_envp));
			}

		}
		char**	convertVectorToChartab(std::vector<std::string>	vectorToConvert) {
			std::vector<char*> argv;
    		for (size_t i = 0; i < vectorToConvert.size(); ++i) {
        		argv.push_back(const_cast<char*>(vectorToConvert[i].c_str()));  // Conversion en char*
    		}
    		argv.push_back(NULL);  // Important : execve() attend un tableau terminé par NULL
			return &argv[0];
		}
};
