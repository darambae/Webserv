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
		std::string	_tempBuffer;
		std::string	_cgiBody;
		int			_cgiContentLength;
		int			_cgiResponseStatus;
		bool		_headerDoneReading;
		std::string _cgiContentType;
		std::string	_requestBody;
		std::map<std::string, std::string>	_cgiHeaders;

	public:
		CgiManager(CGI_env*	cgi_env, Request* request, Response* response);
		int		getSocketsParent() {return _sockets[0];}
		int		getSocketsChildren() {return _sockets[1];}
		int		getCgiResponseStatus() { return _cgiResponseStatus; }
		std::map<std::string, std::string>	getCgiHeaders() const { return _cgiHeaders; }
		std::string	getCgiBody() const { return _cgiBody; }
		int		forkProcess();
		int		sendToCgi();
		int		recvFromCgi();
		void	findContentLength(std::string header);
		void	parseCgiHeader(std::string header);
		pid_t	getPid() {return _children_pid;}
		int		getStatus() {return _children_status;}
		int	check_pid();
		// char**	convertVectorToChartab(std::vector<std::string>	vectorToConvert);
		~CgiManager();

		void setCgiContentType(std::string contentType) { _cgiContentType = contentType; }
		void setCgiContentLength(int contentLength) { _cgiContentLength = contentLength; }
};
