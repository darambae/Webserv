#include "../include/Request.hpp"


//a request is always made of :
// - 1st line containing (in this order) : method (GET/POST/DELETE) ; path (to a file or a directory) ; protocol_version (HTTP/1.1)
// - Header containing extra data
// - Body (the content if needed)

Request::Request(int fd) : isRequestComplete(false), isHeaderRead(false), isRequestPathDirectory(false), _clientFd(fd), _contentLength(0) {
	_time_stamp = get_time();
}

int	Request::parseRequest() {

	char	buffer[1024] = {0};
	// std::cout<<"trying to read fd "<<_clientFd<<std::endl;
	ssize_t	bytes = read(_clientFd, buffer, sizeof(buffer));
	//ssize_t bytes = recv(_clientFd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);

	// std::cout<<"succeed to read fd "<<_clientFd<<std::endl;
	if (bytes < 0) {
		LOG_ERROR("Error reading from client_fd(" + to_string(_clientFd) +")", 1);
		return -1; //client disconnected
	} else if (bytes == 0) {
		LOG_INFO("0 bytes read, Client disconnected");
		return -1;
	}
	//LOG_INFO("request received : \n"+std::string(buffer)+"\n");
	_tempBuffer.append(buffer, bytes);
	//std::cout<< "What's read in buffer : " << buffer<<std::endl;

	//read request's first line if not read yet
	if (_firstLine.empty() && _tempBuffer.find("\r\n") != std::string::npos) {
		size_t pos = _tempBuffer.find("\r\n");
		_firstLine = _tempBuffer.substr(0, pos);
		_tempBuffer.erase(0, pos + 2);
		parseFirstLine();
		LOG_INFO("Request received : " + _firstLine);
	}

	//read request's header
	if (!isHeaderRead && _tempBuffer.find("\r\n\r\n") != std::string::npos) {
		size_t pos = _tempBuffer.find("\r\n\r\n");
		std::string	headerPart = _tempBuffer.substr(0, pos);
		_tempBuffer.erase(0, pos + 4);
		parseHeader(headerPart);
		isHeaderRead = true;
		LOG_INFO("Header received: " + headerPart);
	}

	//if body size is bigger than client max buffer size, we send error 413
	if (_contentLength > 0 && _contentLength > FD_DATA[_clientFd]->server->getConfigServer()->getLimitClientBodySize())
		return 1;

	//read body if present
	if (isHeaderRead && _contentLength > 0) {
		if (_tempBuffer.size() >= static_cast<size_t>(_contentLength)) {
			_body = _tempBuffer.substr(0, _contentLength);
			_tempBuffer.erase(0, _contentLength);
			isRequestComplete = true;
			LOG_INFO("Body received : " + _body);
		}
	}
	else if (isHeaderRead && _contentLength == 0) {
		isRequestComplete = true;
		LOG_INFO("Request parsing complete");
	}

	return 0;
}

void	Request::parseFirstLine() {
	std::istringstream firstLineStream(_firstLine);
	firstLineStream >> _method >> _path >> _version;
	if (_path.find("?") != std::string::npos) {
		_query = parseQueryString();
		createQueryMap(_path);
	}
	if (_path[_path.size() - 1] == '/')
		isRequestPathDirectory = true;
}

void	Request::createQueryMap(std::string path) {
	std::map<std::string, std::string> query_map;
	_path = path.substr(0, path.find("?"));
	std::string query = path.substr(path.find("?") + 1);
	_query = query;
	while (query.find("&") != std::string::npos) {
		std::string key = query.substr(0, query.find("="));
		std::string value = query.substr(query.find("=") + 1, query.find("&") - query.find("=") - 1);
		query_map[key] = value;
		query = query.substr(query.find("&") + 1);
	}
	std::string key = query.substr(0, query.find("="));
	std::string value = query.substr(query.find("=") + 1);
	query_map[key] = value;
	_query_map = query_map;
}

std::string	Request::getValueFromHeader(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator	it = _header.find(key);
	if (it != _header.end())
		return it->second;
	return "";
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
			if (key == "Content-Length") {
				_contentLength = std::atoi(value.c_str());
			}
			else if (key == "Cookie") {
				size_t pos	= value.find("session_id");
				if (pos != std::string::npos) {
					_sessionID = value.substr(pos + 11, pos + 47);
				}
			}
		}
	}
	LOG_INFO("session ID parsed: " + _sessionID);
}

uploadData Request::setFileContent() {
	struct uploadData data;
	std::string content;
	//LOG_INFO("Body to parse: " + _body);
	if (_body.find("filename=") == std::string::npos)
		return data;
	std::string filename = _body.substr(_body.find("filename=") + 10, _body.find("\"\r\n") - (_body.find("filename=") + 10));
	size_t start_pos = _body.find("\r\n\r\n");
	size_t end_pos = _body.find("\r\n----");
	if (start_pos != std::string::npos)
		content = _body.substr(start_pos + 4, end_pos - start_pos - 4);
	data.fileName = filename;
	data.fileContent = content;
	return data;
}

std::string	Request::parseQueryString() {
	std::string	queryString;
	if (_path.find("?") != std::string::npos) {
		size_t	pos = _path.find("?");
		queryString = _path.substr(pos + 1, _path.size() - pos);
		// LOG_INFO("Query = "+queryString);
		return queryString;
	}
	return "";
}

int	Request::handleRequest() {
	LOG_INFO("FD "+to_string(_clientFd)+" received a request");
	ConfigServer* config = FD_DATA[_clientFd]->server->getConfigServer();
	int result_parseRequest = parseRequest();
	if (result_parseRequest == -1) {
		LOG_INFO("Request parsing failed");
		return -1;
	} else if (result_parseRequest == 1) {
		LOG_INFO("Request body too big");
		FD_DATA[_clientFd]->response = new Response(*this, *config);
		FD_DATA[_clientFd]->response->setResponseStatus(413);
		FD_DATA[_clientFd]->response->handleError();
		return 0;
	}

	if (isRequestComplete) {
		FD_DATA[_clientFd]->response = new Response(*this, *config);
		_Response = FD_DATA[_clientFd]->response;
		_Response->handleResponse();
		// LOG_INFO("Response: " + _Response->getReasonPhrase());
	}
	return 0;
}

