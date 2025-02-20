#include "../include/CgiManager.hpp"

CgiManager::CgiManager(CGI_env*	cgi_env, Request* request, Response* response) : _cgi_env(cgi_env), _request(request), _response(response) {
	std::ifstream   python_path("python3_path.txt");
	if (!python_path.is_open())
		LOG_ERROR("The file that has Python3 path can't be opened", 1);
	std::getline(python_path, _python_path);
	python_path.close();

	std::ifstream   php_path("php_path.txt");
	if (!php_path.is_open())
		LOG_ERROR("The file that has php path can't be opened", 1);
	std::getline(php_path, _php_path);
	php_path.close();
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
	server_cgi->addFdData(_sockets[0], "", -1, server_cgi, CGI_parent, false);
	server_cgi->addFdData(_sockets[1], "", -1, server_cgi, CGI_children, false);
	server_cgi->addFdToFds(_sockets[0]);
	server_cgi->addFdToFds(_sockets[1]);
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
		setenv("REQUEST_METHOD", _cgi_env->request_method.c_str(), 1);
		setenv("QUERY_STRING", _cgi_env->query_string.c_str(), 1);
		setenv("CONTENT_LENGTH", _cgi_env->content_length.c_str(), 1);
		setenv("CONTENT_TYPE", _cgi_env->content_type.c_str(), 1);
		setenv("SCRIPT_NAME", _cgi_env->script_name.c_str(), 1);
		setenv("REMOTE_ADDR", _cgi_env->remote_addr.c_str(), 1);
		//std::string fullpath_script = fullPath("data" + _cgi_env->script_name);
		char *argv[] = {const_cast<char *>(("data" + _cgi_env->script_name).c_str()), NULL};
		char *envp[] = {NULL};
		std::string interpreter = _cgi_env->script_name.find(".py") != std::string::npos ? _python_path : _php_path;

		sleep(1);

		execve(interpreter.c_str(), argv, envp);

		//execl(_interpreter.c_str(), _interpreter.c_str(), fullPath(_cgi_env->script_name).c_str(), NULL);
		LOG_ERROR("exec failed", true);
		exit(-1);
	}
	LOG_INFO("CGI parent process, the children pid is "+to_string(pid));
	_children_pid = pid;
	close(_sockets[1]);
	if (_cgi_env->request_method == "GET")
		close(_sockets[0]);

	int	status;
	pid_t	result = waitpid(_children_pid, &status, WNOHANG);
	if (result == 0)
		return LOG_INFO("result of waitpid = 0, means children don't finish"), 0;//children don't finish
	if (result == -1) {//children doesn't exist anymore
		LOG_ERROR("CGI failed, children doesn't exist anymore", false);
		return -1;
	}
	if (result == _children_pid) {
		if (WIFEXITED(status))//if true children finish normally with exit
			if (WEXITSTATUS(status) == -1) {//extract in status the exit status code of children
				LOG_ERROR("CGI failed, execl failed", false);
				return -1;
			}
	}

	return 0;
	//return to the main loop waiting to be able to write or send to cgi
	//after write to send body, if exit == -1, print error message found in socket_cgi[0] and return -1;
}

int	CgiManager::sendToCgi() {//if we enter in this function, it means we have a POLLOUT for CGI_children
	LOG_INFO("POLLOUT flag on the children socket, waiting to recv something");
	int	returnValue = 0;
	if (_cgi_env->request_method == "POST") {
		std::string body = _request->getBody();
		const void* buffer = static_cast<const void*>(body.data());//converti std::string en const void* data
		returnValue = write(_sockets[0], buffer, sizeof(buffer) - 1);
		LOG_DEBUG("returnValue : " + to_string(returnValue));
		close(_sockets[0]);
	}
	return returnValue;
}

int	CgiManager::recvFromCgi() {//if we enter in this function, it means we have a POLLIN for CGI_parent
	LOG_INFO("POLLIN flag on the parent socket, something to read");
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
				LOG_ERROR("CGI failed, execl failed", false);
				return -1;
			}
	}
	char	buffer[1024];
	int	bytes = read(_sockets[0], buffer, sizeof(buffer) - 1);
	if (bytes > 0) {
		buffer[bytes] = '\0';
		LOG_INFO("parent socket read this :\n"+std::string(buffer));
		_response->setBuiltResponse(buffer);
		return bytes;
	}
	else {
		LOG_ERROR("read from CGI failed", true);
		return -1;
	}
}

CgiManager::~CgiManager() {
	if (_cgi_env) {
		delete _cgi_env;
		_cgi_env = NULL;
	}
	LOG_INFO("CGI finish");
}
