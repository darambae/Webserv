#include "../include/Request.hpp"

//a request is always made of :
// - 1st line containing (in this order) : method (GET/POST/DELETE) ; path (/index.html) ; protocol_version (HTTP/1.1)
// - Header containing extra data
// - Body 
void	Request::parseRequest() {

	char	buffer[1024];
	ssize_t	bytes = read(_clientFd, buffer, sizeof(buffer));
	
	if (bytes <= 0) {
		return ; //socket is closed or error. Behavior to define
	}

	_tempBuffer.append(buffer, bytes);

	//read request's first line if not read yet
	if (_firstLine.empty() && _tempBuffer.find("\r\n") != std::string::npos) {
		size_t pos = _tempBuffer.find("\r\n");
		_firstLine = _tempBuffer.substr(0, pos);
		_tempBuffer.erase(0, pos + 2);
		parseFirstLine();
	}

	//read request's header
	if (!isHeaderRead && _tempBuffer.find("\r\n\r\n") != std::string::npos) {
		size_t pos = _tempBuffer.find("\r\n\r\n");
		std::string	headerPart = _tempBuffer.substr(0, pos);
		_tempBuffer.erase(0, pos + 4);
		parseHeader(headerPart);
		isHeaderRead = true;
	}

	//read body if present
	if (isHeaderRead && _contentLength > 0) {
		if (_tempBuffer.size() >= static_cast<size_t>(_contentLength)) {
			_body = _tempBuffer.substr(0, _contentLength);
			_tempBuffer.erase(0, _contentLength);
			isRequestComplete = true;
			//do we reanitialize contentLength to 0 ? maybe further in the code.
		}
	}
	else if (isHeaderRead && _contentLength == 0)
		isRequestComplete = true;
}

void	Request::parseFirstLine() {
	std::istringstream	firstLineStream(_firstLine);
	firstLineStream >> _method >> _path >> _version;

	if (_path[_path.size() - 1] == '/')
		isRequestPathDirectory = true;
}

void	Request::parseHeader(std::string headerPart) {
	std::istringstream stream(headerPart);
	std::string	line;
	while (std::getline(stream, line) && !line.empty()) {
		size_t pos = line.find(": ");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string	value = line.substr(pos + 2);
			_header[key] = value;
			if (key == "Content-Length")
				_contentLength = std::atoi(value.c_str());
		}
	}
}

void	Request::handleRequest(ConfigServer const& config) {

	parseRequest();

	if (isRequestComplete) {
		Response*	response = new Response(*this, config);
		response->handleResponse();
		//delete response;
	}
}