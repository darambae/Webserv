#include "../include/Request.hpp"


//a request is always made of :
// - 1st line containing (in this order) : method (GET/POST/DELETE) ; path (to a file or a directory) ; protocol_version (HTTP/1.1)
// - Header containing extra data
// - Body (the content if needed)
Request::Request(int fd) : isRequestComplete(false), isHeaderRead(false), isRequestPathDirectory(false), _clientFd(fd), _contentLength(0) {
	_time_stamp = get_time();
	_port = -1;
}

int	Request::parseRequest() {

	char	buffer[1024] = {0};
	ssize_t	bytes = read(_clientFd, buffer, sizeof(buffer));

	if (bytes < 0) {
		LOG_ERROR("Error reading from client_fd(" + to_string(_clientFd) +")", 1);
		return -1; //client disconnected
	} else if (bytes == 0) {
		LOG_INFO("0 bytes read, Client disconnected");
		return -1;
	}
	_tempBuffer.append(buffer, bytes);
	//LOG_INFO("BUFFER: " + std::string(buffer));
	//read request's first line if not read yet
	if (_firstLine.empty() && _tempBuffer.find("\r\n") != std::string::npos) {
		size_t pos = _tempBuffer.find("\r\n");
		_firstLine = _tempBuffer.substr(0, pos);
		_tempBuffer.erase(0, pos + 2);
		parseFirstLine();
		LOG_INFO("Request received : " + std::string(GREEN) + _firstLine + RESET);
	}

	//read request's header
	if (!isHeaderRead && _tempBuffer.find("\r\n\r\n") != std::string::npos) {
		size_t pos = _tempBuffer.find("\r\n\r\n");
		std::string	headerPart = _tempBuffer.substr(0, pos);
		_tempBuffer.erase(0, pos + 4);
		parseHeader(headerPart);
		isHeaderRead = true;
		LOG_INFO("Header received:\n" + std::string(GREEN) + headerPart + "\n" + RESET);
		// LOG_DEBUG("Header received:\n" + headerPart + "\n");
	}

	//if body size is bigger than client max buffer size, we send error 413
	if (_contentLength > 0 && _contentLength > FD_DATA[_clientFd]->server->getConfigServer()->getLimitClientBodySize()) {
		isRequestComplete = true;
		return 1;
	}

	//read body if present
	if (isHeaderRead && _contentLength > 0) {
		if (_tempBuffer.size() >= static_cast<size_t>(_contentLength)) {
			_body = _tempBuffer.substr(0, _contentLength);
			_tempBuffer.erase(0, _contentLength);
			isRequestComplete = true;
			LOG_DEBUG("Body received in REQUEST: " + _body);
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
	if (_path.find("?") != std::string::npos)
		_query = parseQueryString();
	if (_path[_path.size() - 1] == '/')
		isRequestPathDirectory = true;
	_path = _path.substr(0, _path.find("?"));
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
					_sessionID = value.substr(pos + 11, pos + 36);
				}
			}
			else if (key == "Host") {
				size_t pos = value.find(":");
				if (pos != std::string::npos) {
					_port = std::atoi(value.substr(pos + 1).c_str());
					value = value.substr(0, pos);
				}
				_hostName = value;
				LOG_INFO("header host : "+value);
			}
		}
	}
}

uploadData Request::setFileContent() {
	struct uploadData data;
	std::string content;

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
		return queryString;
	}
	return "";
}

bool	Request::isIp(std::string host) {
	size_t	pos = host.find(".");
	for (int i = 0; pos != std::string::npos; ++i) {
		int nb = stringToInt(host.substr(0, pos));
		if (nb < 0 || nb > 255)
			return false;
		host = host.substr(pos + 1, host.size() - pos - 1);
		if (i == 2) {
			int nb = stringToInt(host);
			if (nb < 0 || nb > 255)
				return false;
			return true;
		}
		else if (i < 2)
			pos = host.find(".");
		else
			break;
	}
	return false;
}

int	Request::checkHost(ConfigServer* config) {
	std::vector<std::string> serverNames = config->getServerNames();
	serverNames.push_back("localhost");
	if (std::find(serverNames.begin(), serverNames.end(), _hostName) != serverNames.end()
		|| _hostName.empty() || isIp(_hostName))
		return 0;
	return -1;
}

int	Request::handleRequest() {
	ConfigServer* config = FD_DATA[_clientFd]->server->getConfigServer();
	int result_parseRequest = parseRequest();
	if (result_parseRequest == -1)
		return -1;
	if (checkHost(config) == -1) {
		LOG_INFO("bad Request host");
		FD_DATA[_clientFd]->response = new Response(*this, *config);
		FD_DATA[_clientFd]->response->setResponseStatus(400);
		FD_DATA[_clientFd]->response->handleError();
		return 0;
	}
	else if (result_parseRequest == 1) {
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
	}
	return 0;
}

