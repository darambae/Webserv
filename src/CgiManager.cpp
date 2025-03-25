#include "../include/CgiManager.hpp"

CgiManager::CgiManager(CGI_env*	cgi_env, Request* request, Response* response) :
	 _cgi_env(cgi_env), _request(request), _response(response), _headerDoneReading(false) {
	_cgiResponseStatus = 200;
	_children_done = false;
	_children_status = -2;
	_requestBody = "";

	if (_cgi_env->request_method == "POST")
		_requestBody = _request->getBody();
}

char** CgiManager::setEnv() {
    std::vector<std::string> envStrings;
	envStrings.push_back("REQUEST_METHOD=" + _cgi_env->request_method);
    envStrings.push_back("QUERY_STRING=" + _cgi_env->query_string);
    envStrings.push_back("CONTENT_LENGTH=" + _cgi_env->content_length);
    envStrings.push_back("CONTENT_TYPE=" + _cgi_env->content_type);
    envStrings.push_back("SCRIPT_NAME=" + _cgi_env->script_name);
    envStrings.push_back("REMOTE_ADDR=" + _cgi_env->remote_addr);
    envStrings.push_back("HTTP_COOKIE=" + _cgi_env->http_cookie);

    char** env = new char*[envStrings.size() + 1]; 
    for (size_t i = 0; i < envStrings.size(); ++i) {
        env[i] = new char[envStrings[i].size() + 1]; 
        std::strcpy(env[i], envStrings[i].c_str());
    }
    env[envStrings.size()] = NULL; 

    return env;
}

int	CgiManager::forkProcess() {
	if (socketpair(AF_UNIX, SOCK_STREAM /* | SOCK_NONBLOCK | SOCK_CLOEXEC */, 0, _sockets) == -1) {
		LOG_ERROR("socketpair failed", true);
		return -1;
	}
	LOG_INFO("socketpair created : \nFD : "+to_string(_sockets[0])+" is the Parent\nFD : "+to_string(_sockets[1])+" is the Children");
	Server*	server_cgi = FD_DATA[_request->getClientFD()]->server;
	if (server_cgi->unblockFD(_sockets[0]) == -1)
		return -1;
	if (server_cgi->unblockFD(_sockets[1]) == -1)
		return -1;

	server_cgi->addFdData(_sockets[0], "", -1, server_cgi, CGI_parent, _request, _response, this);
	server_cgi->addFdToFds(_sockets[0]);
	if (_cgi_env->request_method == "POST") {//to follow children CGI only if we need to send something
		server_cgi->addFdData(_sockets[1], "", -1, server_cgi, CGI_children, _request, _response, this);
		server_cgi->addFdToFds(_sockets[1]);
	}

	pid_t	pid = fork();
	if (pid == -1) {
		LOG_ERROR("fork failed", true);
		return -1;
	}
	if (pid == 0) {
		close(_sockets[0]);//will be use by parent
		dup2(_sockets[1], STDIN_FILENO);
		dup2(_sockets[1], STDOUT_FILENO);
		close(_sockets[1]);
		std::string interpreter = isFoundIn(_cgi_env->script_name.substr(_cgi_env->script_name.find_last_of(".") + 1), _response->getLocation()->getCgiPass());
		std::string script_path = fullPath("data/cgi-bin/" + _cgi_env->script_name);

		char **env = setEnv();
		char **argv;
		if (interpreter.empty()) {
			argv = new char*[2];
			argv[0] = const_cast<char *>(script_path.c_str());
			argv[1] = NULL;
		} else {
			argv = new char*[3];
			argv[0] = const_cast<char *>(interpreter.c_str());
			argv[1] = const_cast<char *>(script_path.c_str());
			argv[2] = NULL;
		}

		if (execve(argv[0], argv, env) == -1) {
			for (size_t i = 0; env[i] != NULL; ++i) {
				delete[] env[i];
			}
			delete[] env;
			delete[] argv;
			LOG_ERROR("execve failed", true);
			exit(-1);
		}
	}
	sleep(1);
	LOG_INFO("CGI parent process, the children pid is "+to_string(pid));
	_children_pid = pid;
	//check_pid();
	return 0;
	//return to the main loop waiting to be able to write or send to cgi
	//after write to send body, if exit == -1, print error message found in socket_cgi[0] and return -1;
}

int	CgiManager::sendToCgi() {//if we enter in this function, it means we have a POLLOUT for CGI_children
	LOG_INFO("POLLOUT flag on the children socket, waiting to recv something");
	int	returnValue = 0;
	if (_cgi_env->request_method == "POST") {
		LOG_INFO("request Body = "+_requestBody);
		if (_requestBody.size() > 0) {
			const void* buffer = static_cast<const void*>(_requestBody.data());//converti std::string en const void* data
			returnValue = write(_sockets[0], buffer, _requestBody.size());
			if (returnValue > 0 && static_cast<size_t>(returnValue) < _requestBody.size()) {
				_requestBody = _requestBody.substr(returnValue, _requestBody.size() - returnValue);
				return returnValue;
			}
			else if (returnValue <= 0){
				LOG_ERROR("an error occured when Parent sent the body to Children", 1);
				return -1;
			}
		}
	}
	return returnValue;
}


int CgiManager::check_pid() {
	if (_children_done)
		return _children_status;
    pid_t result = waitpid(_children_pid, &_children_status, WNOHANG);
    if (result == 0) {
        return -2; // Child process has not finished
    } else if (result == -1) {
        LOG_ERROR("CGI failed, child process does not exist", false);
        _children_done = true;
		_children_status = -1;
		return _children_status;
    } else {
		_children_done = true;
        if (WIFEXITED(_children_status)) {
            int exit_code = WEXITSTATUS(_children_status);
            _children_status = exit_code; //0~255
        } else if (WIFSIGNALED(_children_status)) {
            int signal = WTERMSIG(_children_status);
			LOG_DEBUG("Child process received this signal : " + to_string(signal));
			_children_status = -1;
        } else
			_children_status = -1;
    }
	return _children_status;
}

void	CgiManager::parseCgiHeader(std::string header) {

	std::istringstream stream(header);
	std::string	line;
	while (std::getline(stream, line) && !line.empty()) {
		size_t pos = line.find(": ");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string	value = line.substr(pos + 2);
			if (key == "Status") {
				_cgiResponseStatus = std::atoi(value.c_str());
				continue;
			}
			else if (key == "Content-Length") {
				_cgiContentLength = std::atoi(value.c_str());
				continue;
			}
			else
				_cgiHeaders[key].push_back(value);
		}
	}
	for (std::map<std::string, std::vector<std::string> >::iterator it = _cgiHeaders.begin(); it != _cgiHeaders.end(); ++it) {
		for (size_t i = 0; i < it->second.size(); i++)
			LOG_INFO("CGI Header stored: " + it->first + " = " + it->second[i]);
	}
}

//if we enter in this function, it means we have a POLLIN for CGI_parent
int	CgiManager::recvFromCgi() {
	//LOG_INFO("POLLIN flag on the parent socket, something to read from child pid ("+ to_string(_children_pid) +")");
	check_pid();

	if (_children_done) {
		char	buffer[1024] = {0};
		int	bytes = read(_sockets[0], buffer, sizeof(buffer) - 1);

		if (bytes <= 0) {
			//LOG_ERROR("reading CGI answer from parent's socket failed", 1);
			return -1;
		}
		else if (bytes > 0) {
			buffer[bytes] = '\0';
			_tempBuffer.append(buffer);

			if (_tempBuffer.find("\r\n\r\n") != std::string::npos) {
				size_t pos = _tempBuffer.find("\r\n\r\n");
				std::string header = _tempBuffer.substr(0, pos + 4);
				_tempBuffer.erase(0, pos + 4);
				parseCgiHeader(header);
				_headerDoneReading = true;
			}

			if (_cgiContentLength > 0) {
				if (_tempBuffer.size() >= static_cast<size_t>(_cgiContentLength)) {
					_cgiBody = _tempBuffer.substr(0, _cgiContentLength);
					_tempBuffer.erase(0, _cgiContentLength);
				}
			}
		}
		if (_cgiBody.size() > 0) {
			LOG_INFO("CGI response done reading");
			_response->setCgiManager(this);
			_response->buildCgiResponse(this);
		}
	}
	return 0;
}

CgiManager::~CgiManager() {
	if (_cgi_env) {
		delete _cgi_env;
		_cgi_env = NULL;
	}
}
