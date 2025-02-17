#pragma once

#include "webserv.hpp"

class Response;

class ResponseBuilder {

	private: 
	
	struct s_headers
	{
		std::string	_timeStamp;
		std::string	_contentType;
		std::string	_contentLength;
	}; 

	Response&		_setup;
	std::string		_builtResponse;
	s_headers		_headers;
	std::string		_body;
	std::map<std::string, std::string>	_mimeTypes;

	public:
	ResponseBuilder(Response& setup): _setup(setup) {}
	~ResponseBuilder() {}

	/* setters/getters */
	std::string const&	getBody() const { return _body; }
	std::string const&	getBuiltResponse() const { return _builtResponse; }
	/* methods */
	void				initMimeTypes();
	std::string*		buildResponse(std::string body);
	std::string			buildFirstLine();
	std::string			buildHeaders();
	std::string			buildTime();
	std::string			buildContentType();

	/* Header building */

};