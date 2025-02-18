
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

struct uploadData {
	std::string		fileName;
	std::string		fileContent;
};

class Request {

	private:

	Response*		_Response;
	bool			isRequestComplete;
	bool			isHeaderRead;
	bool			isRequestPathDirectory;
	int				_clientFd;
	int				_contentLength;
	// CGI data struct to be implemented
	// std::string _content_type;
	std::string _firstLine;
	std::string	_method, _path, _version; //1st line parts (GET/POST/DELETE ; / /index.html ; HTTP/1.1)
	std::string	_tempBuffer;
	std::map<std::string, std::string>	_header;
	std::string	_body;
	std::map<std::string, std::string>	_query_map;
	Request();

public:

	Request(int fd): isRequestComplete(false), isHeaderRead(false), isRequestPathDirectory(false),
					_clientFd(fd), _contentLength(0) {}
	~Request() {}

	struct uploadData	uploadData;
	/* getters */
	std::map<std::string, std::string>	getHeader() const { return _header; }
	std::string	getMethod() const { return _method; }
	std::string	getPath() const { return _path; }
	std::string	getVersion() const { return _version; }
	int			getClientFD() const { return _clientFd; }
	bool		getIsRequestPathDirectory() const { return isRequestPathDirectory; }
	std::string	getValueFromHeader(const std::string& key) const;

	/* setters */
	void	setPath(std::string path) { _path = path; }

	std::string	getBody() const { return _body; }

	/* methods */
	int		handleRequest();
	int		parseRequest();
	void	parseFirstLine();
	void	parseHeader(std::string headerPart);
	struct uploadData	parseBody();
	std::string		parseQueryString();
	void	createQueryMap(std::string path);


};
