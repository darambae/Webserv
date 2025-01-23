#pragma once

#include "Request.hpp"
#include <sys/stat.h>
#include <map>


class Response {

	private:
	ConfigServer const&	_config;
	Request&	_request;
	int			_codeStatus;
	std::string	_reasonPhrase;
	std::map<std::string, std::string>	_header;

	Response();

	public:
	Response(Request& request, ConfigServer const&	config): _request(request), _config(config) {}
	~Response() {}

	/* setters / getters */
	void		setCodeStatus(int code) { _codeStatus = code; }
	int			getCodeStatus() const { return _codeStatus; }
	void		setReasonPhrase(std::string reason) { _reasonPhrase = reason; }
	std::string	getReasonPhrase() const { return _reasonPhrase; }
	void		setHeader(std::string key, std::string value) { _header[key] = value; }
	std::map<std::string, std::string>	getHeader() const { return _header; }

	/* method */
	void	handleResponse();
	ConfigLocation const*	findRequestLocation(ConfigServer const& config, std::string requestPath);
	void	handleGet(ConfigLocation const* location);
	void	handlePost();
	void	handleDelete();
	void	handleError();
};