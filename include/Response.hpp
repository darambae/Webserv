#pragma once

#include "webserv.hpp"
#include "ResponseBuilder.hpp"

class ConfigServer;
class ConfigLocation;
class Request;
class ResponseBuilder;

class Response {



private:
	ResponseBuilder*		_responseBuilder;
	std::string*			_builtResponse;
	ConfigServer const&		_config;
	ConfigLocation const*	_location;
	Request&				_request;
	std::string				_codeStatus;
	std::string				_reasonPhrase;
	std::string				_requestedFilePath;
	std::ifstream			_requestedFile;
	std::map<std::string, std::string>	_header;
	bool					_responseReadyToSend;
	size_t					_totalBytesSent;

	Response();

	public:
	Response(Request& request, ConfigServer const&	config): _config(config), _location(NULL),  _request(request), _responseReadyToSend(false), _totalBytesSent(0) {}
	~Response() {}


	/* setters / getters */
	void		setRequestedFile(std::string const& filePath) {
		_requestedFilePath = filePath;
		_requestedFile.open(filePath.c_str(), std::ios::binary);

	}
	void	setResponseStatus(int code);
	void	setHeader(std::string key, std::string value) { _header[key] = value; }
	void	setBuiltResponse(std::string responseComplete);
	void	setResponseBuilder(ResponseBuilder* responseBuilder) { _responseBuilder = responseBuilder; }
	void	setResponseReadyToSend(bool readyOrNot) { _responseReadyToSend = readyOrNot; }	

	Request 			&getRequest() const { return _request; }
	std::string			getCodeStatus() const { return _codeStatus; }
	std::string const&	getReasonPhrase() const { return _reasonPhrase; }
	std::string			getRequestedFilePath() const { return _requestedFilePath; }
	std::ifstream&		getRequestedFile() { return _requestedFile; }
	bool				getResponseReadyToSend() { return _responseReadyToSend; }
	ResponseBuilder*	getResponseBuilder() const { return _responseBuilder; }
	std::map<std::string, std::string>	getHeader() const { return _header; }

	/* method */
	ConfigLocation const*	findRequestLocation(ConfigServer const& config, std::string requestPath);
	void		handleResponse();
	int			handleCgi();
	int			findIndex();
	int			sendResponse();
	void		handleGet();
	void		handlePost();
	void		handleDelete();
	void		handleError();
	void		buildCgiResponse();
	int			generateDefaultErrorHtml();
	std::string	generateAutoIndex(std::string path);
};
