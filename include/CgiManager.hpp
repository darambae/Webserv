#pragma once

#include "webserv.hpp"

class	CgiManager {
	private:
		std::string					_path;
		std::vector<std::string>	_argv;
		std::vector<std::string>	_envp;
		int							_Fd_client;
		std::string					_dataForCgi;
		int							_sockets[2];
		std::string					_dataForClient;
		bool						_responseReadyToBeSend;
		pid_t						_children_pid;

	public:
		CgiManager(std::string	path_cgi, int fd_client, std::string	dataForCgi);
		int	getSocketsParent() {return _sockets[0];}
		int	getSocketsChildren() {return _sockets[1];}
		int	forkProcess();
		int	sendToCgi();
		int	recvFromCgi();
		char**	convertVectorToChartab(std::vector<std::string>	vectorToConvert);
		~CgiManager();
};
