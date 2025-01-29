#pragma once

#include "SetupResponse.hpp"

class Response {

	private: 
	
	struct s_headers
	{
		std::string	_timeStamp;
		std::string	_contentType;
		std::string	_contentSize;
	}s; 

	SetupResponse&	_setup;
	std::string		_builtResponse;
	s_headers		_headers;
	std::string		_body;
	std::string		_contentType;
	std::string		_contentSize;

	

	public:
	Response(SetupResponse& setup): _setup(setup) {}
	~Response() {}

	/* setters/getters */
	std::string const&	getBody() const { return _body; }
	std::string const&	getBuiltResponse() const { return _builtResponse; }

	/* methods */
	void			buildResponse();
	std::string		buildFirstLine();
	std::string		buildHeaders();
	std::string		buildTime();
	std::string		buildContentType();

	/* Header building */

};