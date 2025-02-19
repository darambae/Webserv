#include "../include/CgiManager.hpp"

CgiManager::CgiManager(CGI_env*	cgi_env, Request* request, Response* response) : _cgi_env(cgi_env), _request(request), _response(response) {}

int	CgiManager::forkProcess() {
	if (socketpair(AF_UNIX, SOCK_STREAM /*| SOCK_NONBLOCK | SOCK_CLOEXEC*/, 0, _sockets) == -1) {
		LOG_ERROR("socketpair failed", true);
		return -1;
	}
	pid_t	pid = fork();
	if (pid == -1) {
		LOG_ERROR("fork failed", true);
		return -1;
	}
	Server*	server_cgi = FD_DATA[_request->getClientFD()]->server;
	server_cgi->addFdData(_sockets[0], "", -1, server_cgi, CGI_parent, false);
	server_cgi->addFdData(_sockets[1], "", -1, server_cgi, CGI_children, false);
	server_cgi->addFdToFds(_sockets[0]);
	server_cgi->addFdToFds(_sockets[1]);
	if (pid == 0) {
		close(_sockets[0]);//will be use by parent
		dup2(_sockets[1], STDIN_FILENO);
		dup2(_sockets[1], STDOUT_FILENO);
		close(_sockets[1]);
		setenv("REQUEST_METHOD", _cgi_env->request_method.c_str(), 1);
		setenv("QUERY_STRING", _cgi_env->query_string.c_str(), 1);
		setenv("CONTENT_LENGTH", _cgi_env->content_length.c_str(), 1);
		setenv("CONTENT_TYPE", _cgi_env->content_type.c_str(), 1);
		setenv("SCRIPT_NAME", _cgi_env->script_name.c_str(), 1);
		setenv("REMOTE_ADDR", _cgi_env->remote_addr.c_str(), 1);
		char	*argv[] = {const_cast<char*>(_cgi_env->script_name.c_str()), NULL};
		char	*envp[] = {NULL};
		LOG_INFO("execve(" + _cgi_env->script_name + ")");
		execve(argv[0], argv, envp);
		LOG_ERROR("exec failed", true);
		exit(-1);
	}
	_children_pid = pid;
	close(_sockets[1]);
	return 0;
	//return to the main loop waiting to be able to write or send to cgi
	//after write to send body, if exit == -1, print error message found in socket_cgi[0] and return -1;
}

int	CgiManager::sendToCgi() {//if we enter in this function, it means we have a POLLOUT for CGI_children
	int	returnValue = 0;
	if (_cgi_env->request_method == "POST") {
		std::string body = _request->getBody();
		const void* buffer = static_cast<const void*>(body.data());//converti std::string en const void* data
		returnValue = write(_sockets[0], buffer, sizeof(buffer) - 1);
		LOG_DEBUG("returnValue : " + to_string(returnValue));
	} else {
		returnValue = write(_sockets[0], _cgi_env->query_string.c_str(), _cgi_env->query_string.size());
		LOG_DEBUG("returnValue : " + to_string(returnValue));
	}
	close(_sockets[0]);
	return returnValue;
}

int	CgiManager::recvFromCgi() {//if we enter in this function, it means we have a POLLIN for CGI_parent
	int	status;
	pid_t	result = waitpid(_children_pid, &status, WNOHANG);
	if (result == 0)
		return 0;//children don't finish
	if (result == -1) {//children doesn't exist anymore
		LOG_ERROR("CGI failed, children doesn't exist anymore", false);
		return -1;
	}
	if (result == _children_pid) {
		if (WIFEXITED(status))//if true children finish normally with exit
			if (WEXITSTATUS(status) == -1) {//extract in status the exit status code of children
				LOG_ERROR("CGI failed, execve failed", false);
				return -1;
			}
	}
	char	buffer[1024];
	int	bytes = read(_sockets[0], buffer, sizeof(buffer) - 1);
	if (bytes > 0) {
		buffer[bytes] = '\0';
		_response->setBuiltResponse(buffer);
		return bytes;
	}
	else {
		LOG_ERROR("read from CGI failed", true);
		return -1;
	}
}

CgiManager::~CgiManager() {
	LOG_INFO("CGI finish");
}
