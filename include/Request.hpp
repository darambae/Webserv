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
		uint64_t		_time_stamp;
		std::string		_savedErrorCode;
		std::string		_sessionID;
		std::string		_firstLine;
		std::string		_method, _path, _version; //1st line parts (GET/POST/DELETE ; / /index.html ; HTTP/1.1)
		std::string		_tempBuffer;
		std::map<std::string, std::string>	_header;
		std::string		_body;
		std::string 	_query;
		std::string		_hostName;
		int				_port;

		Request();

	public:

		Request(int fd);
		~Request() {}

		struct uploadData	uploadData;

		/* getters */
		std::map<std::string, std::string>	getHeader() const { return _header; }
		std::string	getMethod() const { return _method; }
		std::string	getPath() const { return _path; }
		std::string	getSessionID() const { return _sessionID; }
		std::string	getVersion() const { return _version; }
		int			getClientFD() const { return _clientFd; }
		bool		getIsRequestPathDirectory() const { return isRequestPathDirectory; }
		std::string	getValueFromHeader(const std::string& key) const;
		std::string getQuery() const { return _query; }
		uint64_t	getTimeStamp() const { return _time_stamp; }

		/* setters */
		void	setPath(std::string path) { _path = path; }
		void	setTimeStamp(uint64_t time) { _time_stamp = time; }
		std::string	getBody() const { return _body; }

		/* methods */
		int					handleRequest();
		int					parseRequest();
		void				parseFirstLine();
		void				parseHeader(std::string headerPart);
		struct uploadData	setFileContent();
		std::string			parseQueryString();
		int					checkHost(ConfigServer* config);
		bool				isIp(std::string host);
};
