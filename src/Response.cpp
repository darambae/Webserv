#include "../include/Response.hpp"

void	Response::setResponseStatus(int code, std::string reasonPhrase) {
	_codeStatus = to_string(code); _reasonPhrase = reasonPhrase;
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
int	Response::findIndex(ConfigLocation const* location) {

	std::string indexPath = _request.getPath();
	if (!_request.getIsRequestPathDirectory())
		indexPath += "/";
	
	std::vector<std::string> indexesToTry;

	if (location) {
		indexesToTry = location->getIndex();
	} else {
		indexesToTry.push_back("index.html");
	}

	std::string rootPath = fullPath(location ? location->getRoot() : _config.getRoot());

	std::vector<std::string>::const_iterator it = indexesToTry.begin();
	for (; it != indexesToTry.end(); ++it) {
		std::string path = rootPath + indexPath + *it;
		if (access(path.c_str(), F_OK) != -1) {
			LOG_INFO("access to " + path + " success");
			setRequestedFile(path);
			return 1 ;
		}
	}
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
/* void	Response::handleError() {

	std::string	path;
	std::vector<ErrorPage>	errorPages = _config.getErrorPages();

	std::vector<ErrorPage>::const_iterator	it = errorPages.begin();
	for (; it != errorPages.end(); ++it) {
		std::set<int>::const_iterator codesIt = it->error_codes.find(atoi(_codeStatus.c_str()));
		if (codesIt !=  it->error_codes.end()) {
			path = it->error_path;
			break ;
		}
	}
	if (it == errorPages.end()) {
		generateDefaultErrorHtml();
		path = "/defaultErrors";
	}
} */


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
		if (findIndex(location) == 1) {
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

		std::string path = fullPath(location->getRoot() + _request.getPath());
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



void	Response::handlePost() {
	
}

void	Response::handleUpload(ConfigLocation const* location) {
	std::map<std::string, std::string> headers = _request.getHeader();
	struct uploadData fileData = _request.parseBody();
	LOG_INFO("Uploading file : " + fileData.fileName + " THE END OF FILENAME ");
	if (!fileData.fileName.empty() && !fileData.fileContent.empty()) {
		std::string path = fullPath(location->getRoot() + _request.getPath() + "/" + fileData.fileName);
		LOG_INFO("Uploading file to: " + path);
		std::ofstream file(path.c_str(), std::ios::binary);
		file.write(fileData.fileContent.c_str(), fileData.fileContent.size());
		file.close();
		setResponseStatus(200, "OK");
		LOG_INFO("File uploaded successfully");
	}
	else {
		LOG_INFO("Failed to upload the requested file");
		setResponseStatus(400, "Bad request");
	}

}

void	Response::handleResponse() {

	std::string requestPath = _request.getPath();
	std::string requestMethod = _request.getMethod();

	//find the proper location block to read depending on the path given in the request
	ConfigLocation const*	location = findRequestLocation(_config, requestPath);
	
	if (location) {
		//if specific methods are specified in the location block, check if the request's 
		//method match with them
		if (!location->getAllowMethods().empty()) {
			std::set<std::string> allowedMethods = location->getAllowMethods();
			if (allowedMethods.find(requestMethod) == allowedMethods.end()) {
				LOG_INFO("Method not allowed");
				setResponseStatus(405, "Method not allowed");
				//handleError();
				return ;
			}
		}
	} else {
		LOG_INFO("No location found for request path: " + requestPath);
	}

	if (requestMethod == "GET") {
		if (requestPath == "/cgi-bin") {}
			//handleCGI();
		else
			handleGet(location);
	} else if (requestMethod == "POST") {
		if (requestPath == "/cgi-bin") {
			//handleCGI();
		}
		else if (requestPath == "/upload") {
			handleUpload(location);
		} else
			handlePost();
	} else if (requestMethod == "DELETE") {}
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
