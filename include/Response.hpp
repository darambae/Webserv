#pragma once

#include "webserv.hpp"
#include "ResponseBuilder.hpp"

class ConfigServer;
class ConfigLocation;
class Request;
class ResponseBuilder;

class Response {

private:
	ResponseBuilder*	_responseBuilder;
	std::string*		_builtResponse;
	ConfigServer const&	_config;
	Request&			_request;
	std::string			_codeStatus;
	std::string			_reasonPhrase;
	std::string			_requestedFilePath;
	std::ifstream		_requestedFile;
	std::map<std::string, std::string>	_header;
	bool				_responseReadyToSend;
	size_t				_totalBytesSent;

	Response();

	public:
	Response(Request& request, ConfigServer const&	config): _config(config), _request(request), _responseReadyToSend(false), _totalBytesSent(0) {}
	~Response() {}

	/* setters / getters */
	void		setRequestedFile(std::string const& filePath) {
		_requestedFilePath = filePath;
		std::cout<<"\e[31mFILE PATH\e[0m = "<<filePath<<std::endl;
		_requestedFile.open(filePath.c_str(), std::ios::binary);

	}
	void		setResponseStatus(int code, std::string reasonPhrase) { _codeStatus = to_string(code); _reasonPhrase = reasonPhrase; }
	void		setHeader(std::string key, std::string value) { _header[key] = value; }

	Request&			getRequest() const { return _request; }
	std::string			getCodeStatus() const { return _codeStatus; }
	std::string const&	getReasonPhrase() const { return _reasonPhrase; }
	std::string			getRequestedFilePath() const { return _requestedFilePath; }
	std::ifstream&		getRequestedFile() { return _requestedFile; }
	std::map<std::string, std::string>	getHeader() const { return _header; }
	bool				getResponseReadyToSend() {return _responseReadyToSend;}


	/* method */
	ConfigLocation const*	findRequestLocation(ConfigServer const& config, std::string requestPath);
	void	handleResponse();
	bool	findIndex(ConfigLocation const* location);
	int		sendResponse();
	void	handleGet(ConfigLocation const* location);
	void	handlePost();
	void	handleDelete();
	void	handleError();
};
