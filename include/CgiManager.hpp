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
		bool		_children_done;
		int			_children_status;
		std::string	_python_path;
		std::string _php_path;
		std::string	_tempBuffer;
		std::string	_cgiHeader, _cgiBody;
		std::string	_cgiResponse;
		int			_cgiContentLength;
		bool		_headerDoneReading;
		std::string _cgiContentType;
		std::string	_requestBody;

	public:
		CgiManager(CGI_env*	cgi_env, Request* request, Response* response);
		int		getSocketsParent() {return _sockets[0];}
		int		getSocketsChildren() {return _sockets[1];}
		std::string	getCgiHeader() const { return _cgiHeader; }
		std::string	getCgiBody() const { return _cgiBody; }
		int		forkProcess();
		int		sendToCgi();
		int		recvFromCgi();
		void	findContentLength(std::string header);
		pid_t	getPid() {return _children_pid;}
		int	check_pid();
		// char**	convertVectorToChartab(std::vector<std::string>	vectorToConvert);
		~CgiManager();

		void setCgiContentType(std::string contentType) { _cgiContentType = contentType; }
		void setCgiContentLength(int contentLength) { _cgiContentLength = contentLength; }
};
