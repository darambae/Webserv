#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include <sys/stat.h>
#include <map>


class SetupResponse {

	private:
	Response*			_response;
	ConfigServer const&	_config;
	Request&			_request;
	int					_codeStatus;
	std::string			_reasonPhrase;
	std::ifstream		_requestedFile;
	std::map<std::string, std::string>	_header;

	SetupResponse();

	public:
	SetupResponse(Request& request, ConfigServer const&	config): _request(request), _config(config) {}
	~SetupResponse() {}

	/* setters / getters */
	void		setRequestedFile(std::string const& filePath) { _requestedFile.open(filePath.c_str(), std::ios::binary);}
	void		setCodeStatus(int code) { _codeStatus = code; }
	void		setReasonPhrase(std::string reason) { _reasonPhrase = reason; }
	void		setHeader(std::string key, std::string value) { _header[key] = value; }

	int			getCodeStatus() const { return _codeStatus; }
	Request&	getRequest() const { return _request; }
	std::string const&	getReasonPhrase() const { return _reasonPhrase; }
	std::map<std::string, std::string>	getHeader() const { return _header; }
	std::ifstream&	getRequestedFile() { return _requestedFile; }


	/* method */
	ConfigLocation const*	findRequestLocation(ConfigServer const& config, std::string requestPath);
	void	handleResponse();
	bool	findIndex(ConfigLocation const* location);
	void	sendResponse();
	void	handleGet(ConfigLocation const* location);
	void	handlePost();
	void	handleDelete();
	void	handleError();
};