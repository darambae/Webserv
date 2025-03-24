#pragma once
#define LOG_DEBUG(msg) Logger::getInstance(FILE_OUTPUT).log(DEBUG, msg)
#define LOG_INFO(msg) Logger::getInstance(CONSOLE_OUTPUT).log(INFO, msg)
#define LOG_ERROR(msg, errno_set) Logger::getInstance(CONSOLE_OUTPUT).log(ERROR, msg, errno_set)
#define THROW(msg) throw Exception(__FILE__, __FUNCTION__, __LINE__, msg)
#define TIME_OUT 10000 //10s
//colors for logs
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

#include <iostream>
#include <csignal>
#include <fstream>
#include <sstream>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <cstring> 
#include <cstdlib> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <algorithm>
#include <arpa/inet.h>
#include <cerrno>
#include <sys/stat.h>
#include <ctime>
#include <csignal>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/time.h>
#include <fcntl.h>

enum fd_status
{
	CLIENT,
	SERVER,
	CGI_parent,
	CGI_children,
};

#include "Logger.hpp"
#include "Exception.hpp"
#include "ResponseBuilder.hpp"
#include "ServerManager.hpp"
#include "CgiManager.hpp"
#include "ConfigServer.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "ConfigParser.hpp"
#include "ConfigLocation.hpp"
#include "Utils.hpp"
#include "Response.hpp"

class	Server;
class	Request;
class	Response;
class	CgiManager;

struct ErrorPage {
    std::set<int> error_codes;
    std::string error_path;
};

struct CGI_env
{
	std::string	request_method;
	std::string	query_string;//data from the query in URL
	std::string	content_length;
	std::string	content_type;
	std::string	script_name;//path of the CGI executed
	std::string	remote_addr;//client addr
	std::string	http_cookie;

};

struct Fd_data
{
	fd_status		status;
	Server*			server;
	Request*		request;
	Response*		response;
	CgiManager*		CGI;
	int				port;
	std::string		ip;
	bool			just_connected; // In Mac OS to ignore POLLHUP for new clients after new client connection is accepted
};

extern int MAX_CLIENT;//by default but max is defined by system parameters(bash = ulimit -n)
extern std::map<int, Fd_data*>	FD_DATA;
extern std::vector<struct pollfd> ALL_FDS;//stock all fds of all servers & clients & further
extern volatile sig_atomic_t    stopProgram;
