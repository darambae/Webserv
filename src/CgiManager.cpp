#include "../include/CgiManager.hpp"

CgiManager::CgiManager(std::string	path_cgi, int fd_client, std::string	dataForCgi) : _path(path_cgi), _Fd_client(fd_client), _dataForCgi(dataForCgi) {
	_responseReadyToBeSend = false;
}
int	CgiManager::forkProcess() {
	if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK |SOCK_CLOEXEC, 0, _sockets) == -1) {
		LOG_ERROR("socketpair failed", true);
		return -1;
	}
	pid_t	pid = fork();
	if (pid == -1) {
		LOG_ERROR("fork failed", true);
		return -1;
	}
	Server*	server_cgi = FD_DATA[_Fd_client]->server;
	server_cgi->addFdData(_sockets[0], "", -1, server_cgi, CGI_parent, false);
	server_cgi->addFdData(_sockets[1], "", -1, server_cgi, CGI_children, false);
	server_cgi->addFdToFds(_sockets[0]);
	server_cgi->addFdToFds(_sockets[1]);
	if (pid == 0) {
		close(_sockets[0]);//will be use by parent
		dup2(_sockets[1], STDIN_FILENO);
		dup2(_sockets[1], STDOUT_FILENO);
		close(_sockets[1]);
		execve(_path.c_str(), convertVectorToChartab(_argv), convertVectorToChartab(_envp));
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
	const void* buffer = static_cast<const void*>(_dataForCgi.data());//converti std::string en const void* data
	write(_sockets[0], buffer, sizeof(buffer) - 1);
	close(_sockets[0]);
}

int	CgiManager::recvFromCgi() {//if we enter in this function, it means we have a POLLIN for CGI_parent
	int	status;
	pid_t	result = waitpid(_children_pid, &status, WNOHANG);
	if (result == 0)
		return 0;//children don't finish
	if (result == -1)//children doesn't exist anymore
		LOG_ERROR("CGI failed, children doesn't exist anymore", false);
		return -1;
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
		_dataForClient.assign(buffer);
		_responseReadyToBeSend = true;
	}
	else {
		LOG_ERROR("read from CGI failed", true);
		return -1;
	}
}

char**	CgiManager::convertVectorToChartab(std::vector<std::string>	vectorToConvert) {
	std::vector<char*> argv;
	for (size_t i = 0; i < vectorToConvert.size(); ++i) {
		argv.push_back(const_cast<char*>(vectorToConvert[i].c_str()));  // Conversion en char*
	}
	argv.push_back(NULL);  // Important : execve() attend un tableau terminé par NULL
	return &argv[0];
}

CgiManager::~CgiManager() {
	LOG_INFO("CGI finish");
}
