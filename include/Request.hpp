
#pragma once

#include "Response.hpp"
#include "ConfigServer.hpp"
#include "Utils.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <map>

class Response;

class Request {

	private:

	Response*		_Response;
	bool			isRequestComplete;
	bool			isHeaderRead;
	bool			isRequestPathDirectory;
	int				_clientFd;
	int				_contentLength;

	std::string	_firstLine;
	std::string	_method, _path, _version; //1st line parts (GET/POST/DELETE ; / /index.html ; HTTP/1.1)
	std::string	_tempBuffer;
	std::map<std::string, std::string>	_header;
	std::string	_body;
	Request();

	public:

	Request(int fd): isRequestComplete(false), isHeaderRead(false), isRequestPathDirectory(false),
					_clientFd(fd), _contentLength(0) {}
	~Request() { delete _Response; }

	/* getters */
	std::map<std::string, std::string>	getHeader() const { return _header; }
	std::string	getMethod() const { return _path; }
	std::string	getPath() const { return _method; }
	std::string	getVersion() const { return _version; }
	int			getClientFD() const { return _clientFd; }
	bool		getIsRequestPathDirectory() const { return isRequestPathDirectory; }

	/* methods */
	int		handleRequest(ConfigServer const& config);
	int		parseRequest();
	void	parseFirstLine();
	void	parseHeader(std::string headerPart);
};
