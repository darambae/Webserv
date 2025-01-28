#include "../include/SetupResponse.hpp"

ConfigLocation const*	SetupResponse::findRequestLocation(ConfigServer const& config, std::string requestPath) {
	
	const	ConfigLocation*	bestMatch = NULL;
	std::vector<ConfigLocation>::const_iterator	it = config.getLocations().begin();

	for (; it != config.getLocations().end(); ++it) {
		if (requestPath.find(it->getPath()) == 0) {
			if (!bestMatch || it->getPath().size() > bestMatch->getPath().size())
				bestMatch = &(*it);
		}
	}
	return bestMatch;
}


void	SetupResponse::handleResponse() {

	std::string requestPath = _request.getPath();
	std::string requestMethod = _request.getMethod();

	//find the proper location block to read depending on the path given in the request
	ConfigLocation const*	location = findRequestLocation(_config, requestPath);
	if (!location) {
		setCodeStatus(404);
		setReasonPhrase("Not found");
		return ;
	}

	//check if the request's method is allowed in location block of server configuration
	std::set<std::string> allowedMethods = location->getAllowMethods();
	if (allowedMethods.find(requestMethod) == allowedMethods.end()) {
		setCodeStatus(405);
		setReasonPhrase("Method not allowed");
		handleError();
		return ;
	}

	if (requestMethod == "GET")
		handleGet(location);
	else if (requestMethod == "POST")
		handlePost();
	else if (requestMethod == "DELETE")
		handleDelete();
	else {
		setCodeStatus(501);
		setReasonPhrase("method not implemented");
		handleError();
		return ;
	}
}

bool	SetupResponse::findIndex(ConfigLocation const* location) {
	
	std::string indexPath = _request.getPath();

	if (!_request.getIsRequestPathDirectory())
		indexPath += "/";
		
	std::vector<std::string>::const_iterator it = location->getIndex().begin();
	for (; it != location->getIndex().end(); ++it) {
		std::string	tryIndex = *it;
		std::string	tryCompletePath = indexPath + tryIndex;

		if (access(tryCompletePath.c_str(), F_OK) == -1) {
			setRequestedFile(tryCompletePath);
			return true;
		}
	}
	return false;
}

//IF request path is a directory 
//	=> IF index page exist, serve it
//	=> ELSE => IF auto index is ON, file list is generated
//			=> ELSE (auto-index off), error 404 Not found.
//ELSE IF request path is a file
//	=> IF file exist, serve it
//	=> ELSE, error 404 not found
void	SetupResponse::handleGet(ConfigLocation const* location) {

	struct stat	pathStat;
	if (stat((_request.getPath()).c_str(), &pathStat) == 0 && S_ISDIR(pathStat.st_mode)) {
		if (findIndex(location)) {
			setCodeStatus(200);
			setReasonPhrase("OK");
			sendResponse();
		}
		else {
			if (location->getAutoindex()) {
				//generate html page with all the files and repos present in the repo asked in the request
				//page is supposed to be dynamic so we can navigate through website's repos and files via hypertext links
			}
			else {
				setCodeStatus(404);
				setReasonPhrase("Not found");
				handleError();
			}
		}
	}

	else {
	// request path is a file

		setRequestedFile(_request.getPath().c_str());
		if (_requestedFile) {
			setCodeStatus(200);
			setReasonPhrase("OK");
			sendResponse();
		}
		else {
			setCodeStatus(404);
			setReasonPhrase("Not found");
			handleError();
		}
	}

}

void	SetupResponse::sendResponse() {
	
	_response = new Response(*this);
	_response->buildResponse();

}

