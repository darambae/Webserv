#include "../include/CgiManager.hpp"

CgiManager::CgiManager(CGI_env*	cgi_env, Request* request, Response* response) : _cgi_env(cgi_env), _request(request), _response(response), _headerDoneReading(false) {
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
	_requestBody = "";
	if (_cgi_env->request_method == "POST")
		_requestBody = _request->getBody();
}

int	CgiManager::forkProcess() {
	LOG_INFO("Query = "+_cgi_env->query_string);
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
	// server_cgi->addFdData(_sockets[1], "", -1, server_cgi, CGI_children, _request, _response, this);
	// server_cgi->addFdToFds(_sockets[1]);
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
		// LOG_INFO("FULLPATH for SCRIPT : "+fullpath_script);
		setenv("REQUEST_METHOD", _cgi_env->request_method.c_str(), 1);
		setenv("QUERY_STRING", _cgi_env->query_string.c_str(), 1);
		setenv("CONTENT_LENGTH", _cgi_env->content_length.c_str(), 1);
		setenv("CONTENT_TYPE", _cgi_env->content_type.c_str(), 1);
		setenv("SCRIPT_NAME", _cgi_env->script_name.c_str(), 1);
		setenv("REMOTE_ADDR", _cgi_env->remote_addr.c_str(), 1);
		// char *envp[] = {NULL};

		extern char **environ;  // Déclaration de l'environnement global

		std::string interpreter = _cgi_env->script_name.find(".py") != std::string::npos ? _python_path : _php_path;
		// char *argv[] = {const_cast<char *>(interpreter.c_str()), const_cast<char *>(_cgi_env->script_name.c_str()), NULL};
		char script_path[] = "/home/kbrener-/42/WEBSERV/git_webserv_daram/data/cgi-bin/get_player_data.py";
		char *argv[] = {const_cast<char *>(interpreter.c_str()), script_path, NULL};

		sleep(1);

		execve(argv[0], argv, environ);
		// std::cout<<"children try and succeed to communicate with parent process"<<std::endl;

		// std::string html =
        // "<!DOCTYPE html>\n"
        // "<html lang=\"fr\">\n"
        // "<head>\n"
        // "    <meta charset=\"UTF-8\">\n"
        // "    <title>Réponse CGI</title>\n"
        // "</head>\n"
        // "<body>\n"
        // "    <h1>Bienvenue sur mon serveur CGI !</h1>\n"
        // "    <p>Cette page est servie depuis un script CGI.</p>\n"
        // "</body>\n"
        // "</html>\n";

    	// std::cout << "HTTP/1.1 200 OK\r\n";
    	// std::cout << "Content-Type: text/html\r\n";
    	// std::cout << "Content-Length: " << html.size() << "\r\n";
    	// std::cout << "\r\n"; // Séparation entre les headers et le body
    	// std::cout << html;

		//execl(_interpreter.c_str(), _interpreter.c_str(), fullPath(_cgi_env->script_name).c_str(), NULL);
		// LOG_ERROR("exec failed", true);
		exit(-1);
		// exit(0);
	}
	sleep(1);

	LOG_INFO("CGI parent process, the children pid is "+to_string(pid));
	_children_pid = pid;
	return 0;
	//return to the main loop waiting to be able to write or send to cgi
	//after write to send body, if exit == -1, print error message found in socket_cgi[0] and return -1;
}

int	CgiManager::sendToCgi() {//if we enter in this function, it means we have a POLLOUT for CGI_children
	LOG_INFO("POLLOUT flag on the children socket, waiting to recv something");
	int	returnValue = 0;
	if (_cgi_env->request_method == "POST") {
		if (_requestBody.size() > 0) {
			const void* buffer = static_cast<const void*>(_requestBody.data());//converti std::string en const void* data
			returnValue = write(_sockets[0], buffer, _requestBody.size());
			if (returnValue > 0 && static_cast<size_t>(returnValue) < _requestBody.size())
				_requestBody = _requestBody.substr(returnValue, _requestBody.size() - returnValue);
			else
				_requestBody = "";
		}
		LOG_DEBUG("returnValue : " + to_string(returnValue));
	}
	return returnValue;
}

void	CgiManager::findContentLength(std::string header) {

	std::istringstream stream(header);
	std::string	line;
	while (std::getline(stream, line) && !line.empty()) {
		size_t pos = line.find(": ");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string	value = line.substr(pos + 2);
			if (key == "Content-Length")
				_cgiContentLength = std::atoi(value.c_str());
		}
	}
}
int	CgiManager::recvFromCgi() {//if we enter in this function, it means we have a POLLIN for CGI_parent
	LOG_INFO("POLLIN flag on the parent socket, something to read from child pid ("+ to_string(_children_pid) +")"); //<-------ON A FINI LA
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
	char	buffer[1024] = {0};
	int	bytes = read(_sockets[0], buffer, sizeof(buffer) - 1);
	if (bytes < 0) {
		LOG_ERROR("reading CGI answer from parent's socket failed", 1);
		return -1;
	}
	else if (bytes > 0) {
		_tempBuffer.append(buffer);

		if (_tempBuffer.find("\r\n\r\n") != std::string::npos) {
			size_t pos = _tempBuffer.find("\r\n\r\n");
			_cgiHeader = _tempBuffer.substr(0, pos + 4);
			_tempBuffer.erase(0, pos);
			findContentLength(_cgiHeader);
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
		_cgiResponse = _cgiHeader + _cgiBody;
		_response->buildCgiResponse(this);
	}
	return 0;
}

CgiManager::~CgiManager() {
	if (_cgi_env) {
		delete _cgi_env;
		_cgi_env = NULL;
	}
	LOG_INFO("CGI finish");
}
