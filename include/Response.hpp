#pragma once

#include "webserv.hpp"
#include "ResponseBuilder.hpp"

class ConfigServer;
class ConfigLocation;
class Request;
class ResponseBuilder;
class CgiManager;

class Response {

	private:
	ResponseBuilder*		_responseBuilder;
	std::string*			_builtResponse;
	ConfigServer const&		_config;
	ConfigLocation const*	_location;
	CgiManager*				_cgiManager;
	Request&				_request;
	int						_codeStatus;
	std::string				_reasonPhrase;
	std::string				_requestedFilePath;
	std::ifstream			_requestedFile;
	//std::map<std::string, std::string>	_header;
	std::string				_redirectionResponseHeader;
	bool					_responseReadyToSend;
	size_t					_totalBytesSent;

	Response();

	public:
	Response(Request& request, ConfigServer const&	config): _config(config), _location(NULL), _cgiManager(NULL),  _request(request), _codeStatus(200),_responseReadyToSend(false), _totalBytesSent(0) {}
	~Response() {}

	/* setters / getters */
	void	setRequestedFile(std::string const& filePath) {	_requestedFilePath = filePath; }
	void	setResponseStatus(int code);
	void	setBuiltResponse(std::string responseComplete) { *_builtResponse = responseComplete; };
	void	setResponseBuilder(ResponseBuilder* responseBuilder) { _responseBuilder = responseBuilder; }
	void	setResponseReadyToSend(bool readyOrNot) { _responseReadyToSend = readyOrNot; }
	void	setCgiManager(CgiManager* cgiManager) { _cgiManager = cgiManager; }

	Request 				&getRequest() const { return _request; }
	ConfigLocation const*	getLocation() const { return _location; }
	int						getCodeStatus() const { return _codeStatus; }
	std::string const&		getReasonPhrase() const { return _reasonPhrase; }
	std::string				getRequestedFilePath() const { return _requestedFilePath; }
	std::ifstream&			getRequestedFile() { return _requestedFile; }
	bool					getResponseReadyToSend() { return _responseReadyToSend; }
	ResponseBuilder*		getResponseBuilder() const { return _responseBuilder; }
	CgiManager*				getCgiManager() const { return _cgiManager; }
	std::string				getRedirectionResponseHeader() const { return _redirectionResponseHeader; }
	ConfigServer const&		getConfig() const { return _config; }

	/* method */
	ConfigLocation const*	findRequestLocation(ConfigServer const& config, std::string requestPath);
	void		handleResponse();
	int			handleCgi();
	int			findIndex();
	int			sendResponse();
	void		handleGet();
	void		handlePost();
	void		handleUpload(struct uploadData fileData);
	void		handleDelete();
	void		handleError();
	void		handleRedirection();
	void		buildCgiResponse(CgiManager* cgiManager);
	int			generateDefaultErrorHtml();
	std::string	generateAutoIndex(std::string path);
	bool		isRedirectionInfiniteLoop();
};
