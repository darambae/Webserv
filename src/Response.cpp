#include "../include/Response.hpp"

ConfigLocation const*	Response::findRequestLocation(ConfigServer const& config, std::string requestPath) {

	const	ConfigLocation*	bestMatch = NULL;
	std::vector<ConfigLocation>::const_iterator	it = config.getLocations().begin();

	for (; it != config.getLocations().end(); ++it) {
		if (requestPath.find(it->getPath()) != std::string::npos) {
			if (!bestMatch || it->getPath().size() > bestMatch->getPath().size()) {
				bestMatch = &(*it);
			}
		}
	}
	return bestMatch;
}


bool	Response::findIndex(ConfigLocation const* location) {

	std::string indexPath = _request.getPath();
	if (!_request.getIsRequestPathDirectory())
		indexPath += "/";
	std::vector<std::string>::const_iterator it = location->getIndex().begin();
	for (; it != location->getIndex().end(); ++it) {
		std::string	tryIndex = *it;
		std::string	tryCompletePath = indexPath + tryIndex;
		std::string path = fullPath(location->getRoot()) + tryCompletePath;
		if (access(path.c_str(), F_OK) != -1) {
			LOG_INFO("access to " + path + " success");
			setRequestedFile(path);
			return true;
		}
	}
	return false;
}

void		Response::setResponseStatus(int code, std::string reasonPhrase) { _codeStatus = to_string(code); _reasonPhrase = reasonPhrase; }

//IF request path is a directory
//	=> IF index page exist, serve it
//	=> ELSE => IF auto index is ON, file list is generated
//			=> ELSE (auto-index off), error 404 Not found.
//ELSE IF request path is a file
//	=> IF file exist, serve it
//	=> ELSE, error 404 not found
void	Response::handleGet(ConfigLocation const* location) {

	LOG_INFO("Handling GET request");
	struct stat	pathStat;
	LOG_INFO("Request path: " + _request.getPath());
	if (stat((_request.getPath()).c_str(), &pathStat) == 0 && S_ISDIR(pathStat.st_mode)) {
		if (findIndex(location)) {
			setResponseStatus(200, "OK");
			LOG_INFO("Index found");
			_responseReadyToSend = true;
			_responseBuilder = new ResponseBuilder(*this);
			_builtResponse = _responseBuilder->buildResponse();
			LOG_INFO("Response built:\n" + *_builtResponse);
		}
		else {
			if (location->getAutoindex()) {
				//generate html page with all the files and repos present in the repo asked in the request
				//page is supposed to be dynamic so we can navigate through website's repos and files via hypertext links
			}
			else {
				setResponseStatus(403, "Forbidden");
				//handleError();
			}
		}
	}
	else {
	// request path is a file
		// std::string root = location->getRoot().empty() ? _config.getRoot(): location->getRoot();
		// std::string	path = fullPath(root + _request.getPath());
		std::string path = fullPath(location->getRoot() + _request.getPath());
		// LOG_INFO("Request file path: " + path);
		// LOG_INFO("Request file root: " + root);
		setRequestedFile(path.c_str());
		if (_requestedFile) {
			setResponseStatus(200, "OK");
			_responseReadyToSend = true;
			_responseBuilder = new ResponseBuilder(*this);
			_builtResponse = _responseBuilder->buildResponse();
			LOG_INFO("Response built:\n" + *_builtResponse);
		}
		else {
			setResponseStatus(404, "Not found");
			//handleError();
		}
	}
}


void	Response::handleResponse() {

	std::string requestPath = _request.getPath();
	std::string requestMethod = _request.getMethod();

	//find the proper location block to read depending on the path given in the request
	ConfigLocation const*	location = findRequestLocation(_config, requestPath);
	if (!location) {
		LOG_INFO("No location found for request path: " + requestPath);
		setResponseStatus(404, "Not found");
		return ;
	}

	//check if the request's method is allowed in location block of server configuration
	std::set<std::string> allowedMethods = location->getAllowMethods();
	if (allowedMethods.find(requestMethod) == allowedMethods.end()) {
		LOG_INFO("Method not allowed");
		setResponseStatus(405, "Method not allowed");
		//handleError();
		return ;
	}

	if (requestMethod == "GET")
		handleGet(location);
	else if (requestMethod == "POST") {}
		//handlePost();
	else if (requestMethod == "DELETE") {}
		//handleDelete();
	else {
		LOG_INFO("Method not implemented");
		setResponseStatus(501, "Method not implemented");
		//handleError();
		return ;
	}
}

int	Response::sendResponse() {

	size_t	responseSize = _builtResponse->size();
	const size_t	BUFFER_SIZE = 4096;

	LOG_INFO("sending response");
	if (_totalBytesSent < responseSize) {
		size_t bytesToSend = std::min(responseSize - _totalBytesSent, BUFFER_SIZE);
		ssize_t bytesSent = send(_request.getClientFD(), _builtResponse->c_str() + _totalBytesSent, bytesToSend, 0);

		if (bytesSent <= 0) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return -1;
			}
			LOG_INFO("Client disconnected");
			_responseReadyToSend = false;
			return -1;
		}

		_totalBytesSent += bytesSent;
	}

	if (_totalBytesSent == responseSize) {
		LOG_INFO("Response fully sent");
		_responseReadyToSend = false;
	}

	if (_responseBuilder) {
		delete _responseBuilder;
		_responseBuilder = NULL;
	}

	return 0;
}
