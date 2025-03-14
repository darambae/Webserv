#include "../include/Response.hpp"

void	Response::setResponseStatus(int code) {
	_codeStatus = to_string(code);

	switch (code) {
		case 200: _reasonPhrase = "OK"; break;
		case 301: _reasonPhrase = "Moved Permanently"; break;
		case 302: _reasonPhrase = "Found"; break;
		case 400: _reasonPhrase = "Bad Request"; break;
		case 403: _reasonPhrase = "Forbidden"; break;
		case 404: _reasonPhrase = "Not Found"; break;
		case 405: _reasonPhrase = "Method Not Allowed"; break;
		case 408: _reasonPhrase = "Request Timeout"; break;
		case 413: _reasonPhrase = "Payload Too Large"; break;
		case 418: _reasonPhrase = "Unsupported Media Type"; break;
		case 422: _reasonPhrase = "Unprocessable Entity"; break;
		case 500: _reasonPhrase = "Internal Server Error"; break;
		case 501: _reasonPhrase = "Not Implemented"; break;
		case 502: _reasonPhrase = "Bad Gateway"; break;

	}

}

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

//find index page and set it up to be send in response if found (return 1).
//Otherwise, return 0
int	Response::findIndex() {

	std::string indexPath = _request.getPath();
	if (!_request.getIsRequestPathDirectory())
		indexPath += "/";

	std::vector<std::string> indexesToTry;

	if (_location) {
		indexesToTry = _location->getIndex();
	} else {
		indexesToTry.push_back("index.html");
	}

	std::string rootPath = fullPath(_location ? _location->getRoot() : _config.getRoot());

	std::vector<std::string>::const_iterator it = indexesToTry.begin();
	for (; it != indexesToTry.end(); ++it) {
		std::string path = rootPath + indexPath + *it;
		if (access(path.c_str(), F_OK) != -1) {
			LOG_INFO("access to " + path + " success");
			setRequestedFile(path);
			_requestedFile.open(path.c_str(), std::ios::binary);
			return 1 ;
		}
	}
	return 0;
}

int	Response::generateDefaultErrorHtml() {

	std::string	directory = "defaultError";
	struct stat	info;

	if (stat(directory.c_str(), &info) != 0) {
		if (mkdir(directory.c_str(), 0755) != 0) {
			LOG_ERROR("couldn't create defaultError directory", 0);
			return -1;
		}
	}

	std::ofstream	file((directory + "/defaultError.html").c_str());

	if (!file) {
		LOG_ERROR("defaultError.html couldn't be created", 0);
		return -1;
	}

	file << "<!DOCTYPE html>\n"
			<< "<html>\n"
			<< "<head>\n"
			<< "    <meta charset=\"UTF-8\">\n"
			<< "    <title>Erreur " << _codeStatus << "</title>\n"
			<< "    <style>\n"
			<< "        body { font-family: Arial, sans-serif; text-align: center; padding: 50px; }\n"
			<< "        h1 { color: red; }\n"
			<< "    </style>\n"
			<< "</head>\n"
			<< "<body>\n"
			<< "    <h1>Erreur " << _codeStatus << " - " << _reasonPhrase << "</h1>\n"
			<< "    <p>Requested page has encountered a problem</p>\n"
			<< "</body>\n"
			<< "</html>\n";

	file.close();

	return 0;
}

//IF error_page URI (FI /errors/404NotFound.html) defined in location block
//	=>	reprocess like it's a requestPath : find the location block to get the proper
//		root path (or default one if no root defined in the location block),
//		make a full path with it and build the response.
//ELSE IF error_page URI is defined in server block
//	=> same reprocess as above.
//ELSE
//	-> serve default one.
void	Response::handleError() {

	std::string	path;
	std::vector<ErrorPage>	errorPages = (_location && !_location->getErrorPages().empty()) ? _location->getErrorPages() : _config.getErrorPages();
	bool	errorPageFound = false;

	if (!errorPages.empty()) {
		std::vector<ErrorPage>::const_iterator	it = errorPages.begin();
		for (; it != errorPages.end(); ++it) {
			std::set<int>::const_iterator errorCodesIt = it->error_codes.find(atoi(_codeStatus.c_str()));
			if (errorCodesIt !=  it->error_codes.end()) {
				_request.setPath(it->error_path);
				errorPageFound = true;
				LOG_INFO("error_path: " + it->error_path);
				_location = findRequestLocation(_config, _request.getPath());
				LOG_INFO("Location block for error: " + _location->getPath());
				handleGet();
				break ;
			}
		}
	}
	if (!errorPageFound) {
		if (generateDefaultErrorHtml() == 0) {
			path = fullPath("/defaultError/defaultError.html");
			setRequestedFile(path);
			_requestedFile.open(path.c_str(), std::ios::binary);
			_responseReadyToSend = true;
			_responseBuilder = new ResponseBuilder(*this);
			_builtResponse = _responseBuilder->buildResponse("");
		}
	}
}

std::string	Response::generateAutoIndex(std::string path) {

	std::stringstream	html;
	html << "<!DOCTYPE html>\n<html>\n<head>\n";
	html << "<title>Index of " << path << "</title>\n</head>\n<body>\n";
	html << "<h1>Index of " << path << "</h1>\n<ul>\n";

	DIR *dir = opendir(path.c_str());
	if (!dir) {
		LOG_ERROR("failed to open directory: " + path, 1);
		setResponseStatus(500);
		handleError();
		return NULL;
	}

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		std::string	fileName = entry->d_name;
		if (fileName == ".")
			continue;
		if (fileName == "..")
			html << "<li><a href=\"../\">../ (Parent Directory)</a></li>\n";
		else
			html << "<li><a href=\"" << fileName << "\">" << fileName << "</a></li>\n";
	}
	closedir(dir);
	html << "</ul>\n</body>\n</html>\n";

	return html.str();
}

//IF request path is a directory
//	=> IF index page exist, serve it
//	=> ELSE => IF auto index is ON, file list is generated
//			=> ELSE (auto-index off), error 403 Forbidden.
//ELSE IF request path is a file
//	=> IF file exist, serve it
//	=> ELSE, error 404 not found
void	Response::handleGet() {

	LOG_INFO("Handling GET request");
	struct stat	pathStat;
	std::string	requestPath = _request.getPath();
	std::string rootPath = fullPath(_location ? _location->getRoot() : _config.getRoot());
	std::string path = rootPath + requestPath;
	LOG_INFO("rootPath: " + rootPath);
	LOG_INFO("request path before concatenation: " + requestPath);
	//LOG_INFO("path: " + path);

	if (stat(path.c_str(), &pathStat) == 0 && S_ISDIR(pathStat.st_mode)) {
	// request path is a directory
		if (findIndex() == 1) {
			setResponseStatus(200);
			LOG_INFO("Index found");
			_responseReadyToSend = true;
			_responseBuilder = new ResponseBuilder(*this);
			_builtResponse = _responseBuilder->buildResponse("");
			//LOG_INFO("Response built:\n" + *_builtResponse);
		} else {
			LOG_INFO("No index file found, checking if auto-index enable");
			if (_location->getAutoindex()) {
				LOG_INFO("Autoindex enabled, generating directory listing...");
				std::string autoIndexPage = generateAutoIndex(path);
				setResponseStatus(200);
				_responseReadyToSend = true;
				_responseBuilder = new ResponseBuilder(*this);
				_builtResponse = _responseBuilder->buildResponse(autoIndexPage);
			}
			else {
				setResponseStatus(403);
				handleError();
			}
		}
	}
	else {
	// request path is a file
		//LOG_INFO("Path is not a directory, treating as a file : " + requestPath);
		//If the file exists, set and serve it
		if (!rootPath.empty() && stat(path.c_str(), &pathStat) == 0 && S_ISREG(pathStat.st_mode)) {
			//LOG_INFO("File found");
			setRequestedFile(path.c_str());
			//If the file is not a .json file, serve it
			if (requestPath.find(".json") != std::string::npos) {
				setResponseStatus(403);
				handleError();
			} else if (!isFoundIn(requestPath.substr(requestPath.find_last_of(".")), _location->getCgiExtension()).empty()) {
				if (handleCgi() == -1) {
					setResponseStatus(415);
					handleError();
				}
			} else {
				//LOG_INFO("path for the requested file : " + path);
				_requestedFile.open(path.c_str(), std::ios::binary);
				setResponseStatus(200);
				_responseReadyToSend = true;
				_responseBuilder = new ResponseBuilder(*this);
				_builtResponse = _responseBuilder->buildResponse("");
			}
		} else {
			//LOG_INFO("File not found");
			setResponseStatus(404);
			handleError();
		}
	}
}


void	Response::handlePost() {
	LOG_INFO("Handling POST request");
	struct uploadData fileData = _request.setFileContent();
	//Post request with no file content
	if (fileData.fileName.empty() && fileData.fileContent.empty()) {
		LOG_INFO("Post request with no file content");
		setResponseStatus(200);
		_responseReadyToSend = true;
		_responseBuilder = new ResponseBuilder(*this);
		std::ostringstream responseBody;
		responseBody << "<!DOCTYPE html>\n";
		responseBody << "<html lang=\"en\">\n";
		responseBody << "<head>\n";
		responseBody << "<meta charset=\"UTF-8\">\n";
		responseBody << "<title>Post request received</title>\n";
		responseBody << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/style.css\">\n";
		responseBody << "</head>\n";
		responseBody << "<body>\n";
		responseBody << "<h1>Post request received</h1>\n";
		responseBody << "<p>Body content : " << _request.getBody() << "</p>\n";
		responseBody << "</body>\n";
		responseBody << "</html>\n";
		_builtResponse = _responseBuilder->buildResponse(responseBody.str());
		return ;
	}
	//Post request with img file content
	if (fileData.fileName.find(".jpg") == std::string::npos && fileData.fileName.find(".jpeg") == std::string::npos && fileData.fileName.find(".png") == std::string::npos) {
		setResponseStatus(400);
		handleError();
		return ;
	}
	std::string upload_location = _location->getRoot() + _request.getPath();
	std::string path = fullPath(upload_location) + "/" + fileData.fileName;
	std::ofstream file(path.c_str(), std::ios::binary);
	if (!file.is_open()) {
		LOG_INFO("Failed to upload the requested file");
		setResponseStatus(400);
		handleError();
		return ;
	}
	file.write(fileData.fileContent.c_str(), fileData.fileContent.size());
	if (file.fail()) {
		LOG_INFO("Failed to upload the requested file");
		setResponseStatus(400);
		handleError();
		return ;
	}
	file.close();
	setResponseStatus(201);
	LOG_INFO("File uploaded successfully");
	_responseReadyToSend = true;
	_responseBuilder = new ResponseBuilder(*this);
	std::ostringstream responseBody;
	responseBody << "<!DOCTYPE html>\n";
	responseBody << "<html lang=\"en\">\n";
	responseBody << "<head>\n";
	responseBody << "<meta charset=\"UTF-8\">\n";
	responseBody << "<title>File Upload Success</title>\n";
	responseBody << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/style.css\">\n";
	responseBody << "</head>\n";
	responseBody << "<body>\n";
	responseBody << "<h1>File Upload Successful</h1>\n";
	responseBody << "<p>Your file has been uploaded successfully.</p>\n";
	responseBody << "<p>File Name: <strong>" << fileData.fileName << "</strong></p>\n";
	responseBody << "<img src=\"" << _request.getPath() << "/" << fileData.fileName << "\" alt=\"" << fileData.fileName << "\">\n";
	responseBody << "<form action=\"" << _request.getPath() << "/" << fileData.fileName << "\" method=\"delete\">\n";
	responseBody << "<input type=\"hidden\" name=\"_method\" value=\"DELETE\">\n";
	responseBody << "<input type=\"hidden\" name=\"fileName\" value=\"" << fileData.fileName << "\">\n";
	responseBody << "<div class=\"button-container\">\n";
	responseBody << "<button type=\"submit\" class=\"delete-button\">x</button>\n";
	responseBody << "<a href=\"/\" class=\"button\">Go to Index Page</a>\n";
	responseBody << "</form>\n";
	responseBody << "</body>\n";
	responseBody << "</html>\n";
	//LOG_INFO("Response body stream : " + responseBody.str());
	_builtResponse = _responseBuilder->buildResponse(responseBody.str());
	//LOG_INFO("Response built:\n" + _responseBuilder->getBuiltResponse());
}

void	Response::handleDelete() {
	LOG_INFO("Handling DELETE request");
	std::string path = fullPath(_location->getRoot());
	path += _request.getPath();
	//LOG_INFO("Path to delete: " + path);
	if (remove(path.c_str()) != 0) {
		LOG_INFO("Failed to delete the requested file");
		setResponseStatus(400);
		handleError();
		return ;
	}
	setResponseStatus(200);
	LOG_INFO("File deleted successfully");
	_responseReadyToSend = true;
	_responseBuilder = new ResponseBuilder(*this);
	std::ostringstream responseBody;
	responseBody << "<!DOCTYPE html>\n";
	responseBody << "<html lang=\"en\">\n";
	responseBody << "<head>\n";
	responseBody << "<meta charset=\"UTF-8\">\n";
	responseBody << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/style.css\">\n";
	responseBody << "<title>File Delete Success</title>\n";
	responseBody << "</head>\n";
	responseBody << "<body>\n";
	responseBody << "<h1>File Delete Successful</h1>\n";
	responseBody << "<p>Your file has been deleted successfully.</p>\n";
	responseBody << "<p>File Name: <strong>" << _request.getPath().substr(_request.getPath().find_last_of("/") + 1) << "</strong></p>\n";
	responseBody << "<p><a href=\"/\" class=\"button\">Go to Index Page</a></p>\n";
	responseBody << "</body>\n";
	responseBody << "</html>\n";
	_builtResponse = _responseBuilder->buildResponse(responseBody.str());
}

void	Response::handleResponse() {

	std::string requestPath = _request.getPath();
	std::string requestMethod = _request.getMethod();
	//find the proper location block to read depending on the path given in the request
	_location = findRequestLocation(_config, requestPath);

	if (_location) {
		//if specific methods are specified in the location block, check if the request's
		//method match with them
		if (!_location->getAllowMethods().empty()) {
			std::set<std::string> allowedMethods = _location->getAllowMethods();
			if (allowedMethods.find(requestMethod) == allowedMethods.end()) {
				LOG_INFO("Method not allowed");
				setResponseStatus(405);
				handleError();
				return ;
			}
		}
		else if (!_location->getReturn().empty()) {
			std::map<int, std::string>::const_iterator it = _location->getReturn().begin();
			setResponseStatus(it->first);
			_responseReadyToSend = true;
			_responseBuilder = new ResponseBuilder(*this);
			_builtResponse = _responseBuilder->buildResponse("");
		}
	} else {
		LOG_INFO("No location found for request path: " + requestPath);
	}

	LOG_INFO("requestPath: " + requestPath);
	if (requestMethod == "GET") {
		if (!_request.getQuery().empty() && _request.getQuery().find("_method=DELETE") != std::string::npos)
			handleDelete();
		else
			handleGet();
	} else if (requestMethod == "POST") {
		if (requestPath.find(".") != std::string::npos && !isFoundIn(requestPath.substr(requestPath.find_last_of(".")), _location->getCgiExtension()).empty()) {
			LOG_INFO("Handling POST request with CGI");
			if (handleCgi() == -1) {
				setResponseStatus(666);
				handleError();
				return ;
			}
		}
		else
			handlePost();
	} else if (requestMethod == "DELETE")
		handleDelete();
	else {
		LOG_INFO("Method not implemented");
		setResponseStatus(501);
		handleError();
		return ;
	}
}

int	Response::handleCgi() {
	CGI_env*	cgi = new CGI_env;
	cgi->request_method = _request.getMethod();
	if (cgi->request_method == "GET") {
		cgi->content_length = "";
		cgi->content_type = "";
		cgi->query_string = _request.getQuery();
	}
	else {
		cgi->content_length = _request.getValueFromHeader("Content-Length");
		//LOG_INFO("Content-Length: " + cgi->content_length);
		cgi->content_type = _request.getValueFromHeader("Content-Type");
		cgi->query_string = !_request.getBody().empty() ? _request.getBody() : "";
	}
	cgi->remote_addr = FD_DATA[_request.getClientFD()]->ip;
	cgi->http_cookie = _request.getSessionID();
	LOG_DEBUG("session id stack for cgi env is : "+cgi->http_cookie);
	cgi->script_name = _request.getPath().substr(_request.getPath().find_last_of("/") + 1);
	//LOG_DEBUG("CGI script name: " + cgi->script_name);
	FD_DATA[_request.getClientFD()]->CGI = new CgiManager(cgi, &_request, this);
	return FD_DATA[_request.getClientFD()]->CGI->forkProcess();
}

void	Response::buildCgiResponse(CgiManager* cgiManager) {

	LOG_INFO("Building CGI response...");
	setResponseStatus(cgiManager->getCgiResponseStatus());
	if (std::atoi(_codeStatus.c_str()) >= 400) {
		handleError();
		return ;
	}
	_responseBuilder = new ResponseBuilder(*this);
	_responseReadyToSend = true;
	_builtResponse = _responseBuilder->buildResponse(cgiManager->getCgiBody());
	LOG_INFO("Built Response: " + *_builtResponse);
}

int	Response::sendResponse() {

	size_t	responseSize = _builtResponse->size();
	const size_t	BUFFER_SIZE = 4096;
	//LOG_INFO("sending response");
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
		if (_responseBuilder) {
			delete _responseBuilder;
			_responseBuilder = NULL;
		}
	}
	return 1;
}
