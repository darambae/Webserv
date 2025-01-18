
#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <map>

class Request {

	private:

	bool		isRequestComplete;
	bool		isHeaderRead;
	int			_clientFd;
	int			_contentLength;

	std::string	_firstLine;
	std::string	_method, _path, _version; //1st line parts (GET/POST/DELETE ; / /index.html ; HTTP/1.1)
	std::string	_tempBuffer;
	std::map<std::string, std::string>	_header;
	std::string	_body;
	Request() {}

	public:

	Request(int fd): _clientFd(fd), isRequestComplete(false), isHeaderRead(false), _contentLength(0) {}
	~Request() {}

	/* getters */
	std::map<std::string, std::string>	getHeader() const { return _header; }
	std::string	getMethod() const { return _path; }
	std::string	getPath() const { return _method; }
	std::string	getVersion() const { return _version; }

	/* methods */
	void	parseRequest();
	void	parseFirstLine();
	void	parseHeader(std::string headerPart);
};
