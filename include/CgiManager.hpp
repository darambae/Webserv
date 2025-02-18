/* #pragma once

#include "webserv.hpp"

class	CgiManager {
	private:
		CGI_env*	_cgi_env;
		Request*	_request;
		Response*	_response;
		int			_sockets[2];
		pid_t		_children_pid;

	public:
		CgiManager(CGI_env*	cgi_env, Request* request, Response* response);
		int	getSocketsParent() {return _sockets[0];}
		int	getSocketsChildren() {return _sockets[1];}
		int	forkProcess();
		int	sendToCgi();
		int	recvFromCgi();
		// char**	convertVectorToChartab(std::vector<std::string>	vectorToConvert);
		~CgiManager();
}; */
