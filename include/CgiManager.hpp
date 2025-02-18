#pragma once

#include "webserv.hpp"

class Request;
class Response;
struct CGI_env;
class	CgiManager {
	private:
		CGI_env*	_cgi_env;
		Request*	_request;
		Response*	_response;
		int			_sockets[2];
		pid_t		_children_pid;
		std::string	_interpreter;

	public:
		CgiManager(CGI_env*	cgi_env, Request* request, Response* response);
		int	getSocketsParent() {return _sockets[0];}
		int	getSocketsChildren() {return _sockets[1];}
		int	forkProcess();
		int	sendToCgi();
		int	recvFromCgi();
		pid_t	getPid() {return _children_pid;}
		// char**	convertVectorToChartab(std::vector<std::string>	vectorToConvert);
		~CgiManager();
};
