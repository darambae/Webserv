#pragma once


#include <iostream>
#include <csignal>
#include <fstream>
#include <sstream>
#include <list>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <cstring> //memset...
#include <cstdlib> //exit ...
#include <sys/socket.h> // socket, bind, accept...
#include <netinet/in.h> // sockaddr_in
#include <unistd.h> //close...
#include <poll.h>
#include <algorithm>
#include <arpa/inet.h>
#include <cerrno>
#include <sys/stat.h>
#include <ctime>
#include <exception>
#include <csignal>
#include <sys/wait.h>

enum fd_status
{
	CLIENT,
	SERVER,
};

#include "ServerManager.hpp"
#include "ConfigServer.hpp"
#include "Response.hpp"
#include "Exception.hpp"
#include "Logger.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "ConfigParser.hpp"
#include "ConfigLocation.hpp"
#include "Utils.hpp"
#include <fcntl.h>

#define LOG_DEBUG(msg) Logger::getInstance(FILE_OUTPUT).log(DEBUG, msg)
#define LOG_INFO(msg) Logger::getInstance(CONSOLE_OUTPUT).log(INFO, msg)
#define LOG_ERROR(msg, errno_set) Logger::getInstance(CONSOLE_OUTPUT).log(ERROR, msg, errno_set)
#define THROW(msg) throw Exception(__FILE__, __FUNCTION__, __LINE__, msg)

class	Server;
class	Request;
class	Response;

struct ErrorPage {
    std::set<int> error_codes;
    std::string error_path;
};



struct Fd_data
{
	fd_status	status;
	Server*			server;
	Request*		request;
	Response*		response;
	int				port;
	std::string		ip;
};

extern int MAX_CLIENT;//by default but max is defined by system parameters(bash = ulimit -n)
extern std::map<int, Fd_data*>	FD_DATA;
extern std::vector<struct pollfd> ALL_FDS;//stock all fds of all servers & clients & further
extern volatile sig_atomic_t    stopProgram;


// enum HttpStatus {
//     // Informational responses (100–199)
//     CONTINUE = 100,
//     SWITCHING_PROTOCOLS = 101,
//     PROCESSING = 102,                // WebDAV
//     EARLY_HINTS = 103,

//     // Successful responses (200–299)
//     OK = 200,
//     CREATED = 201,
//     ACCEPTED = 202,
//     NON_AUTHORITATIVE_INFORMATION = 203,
//     NO_CONTENT = 204,
//     RESET_CONTENT = 205,
//     PARTIAL_CONTENT = 206,
//     MULTI_STATUS = 207,              // WebDAV
//     ALREADY_REPORTED = 208,          // WebDAV
//     IM_USED = 226,                   // HTTP Delta Encoding

//     // Redirection messages (300–399)
//     MULTIPLE_CHOICES = 300,
//     MOVED_PERMANENTLY = 301,
//     FOUND = 302,                     // Previously "Moved Temporarily"
//     SEE_OTHER = 303,
//     NOT_MODIFIED = 304,
//     USE_PROXY = 305,                 // Deprecated
//     TEMPORARY_REDIRECT = 307,
//     PERMANENT_REDIRECT = 308,

//     // Client error responses (400–499)
//     BAD_REQUEST = 400,
//     UNAUTHORIZED = 401,
//     PAYMENT_REQUIRED = 402,          // Reserved for future use
//     FORBIDDEN = 403,
//     NOT_FOUND = 404,
//     METHOD_NOT_ALLOWED = 405,
//     NOT_ACCEPTABLE = 406,
//     PROXY_AUTHENTICATION_REQUIRED = 407,
//     REQUEST_TIMEOUT = 408,
//     CONFLICT = 409,
//     GONE = 410,
//     LENGTH_REQUIRED = 411,
//     PRECONDITION_FAILED = 412,
//     PAYLOAD_TOO_LARGE = 413,
//     URI_TOO_LONG = 414,
//     UNSUPPORTED_MEDIA_TYPE = 415,
//     RANGE_NOT_SATISFIABLE = 416,
//     EXPECTATION_FAILED = 417,
//     IM_A_TEAPOT = 418,               // Easter Egg (RFC 2324)
//     MISDIRECTED_REQUEST = 421,       // HTTP/2
//     UNPROCESSABLE_ENTITY = 422,      // WebDAV
//     LOCKED = 423,                    // WebDAV
//     FAILED_DEPENDENCY = 424,         // WebDAV
//     TOO_EARLY = 425,                 // Experimental
//     UPGRADE_REQUIRED = 426,
//     PRECONDITION_REQUIRED = 428,     // RFC 6585
//     TOO_MANY_REQUESTS = 429,         // RFC 6585
//     REQUHEADER_FIELDS_TOO_LARGE = 431, // RFC 6585
//     UNAVAILABLE_FOR_LEGAL_REASONS = 451,   // RFC 25

//  // Servinter error responses (500–599)
//    INTER_SERVER_ERROR = 500,
//    NOT_IMPLETED = 501,
//    BAD_GWAY = 502,
// SERVICE_UNAVAILABLE = 503,
//     GATEWAY_TIMEOUT = 504,
//     HTTP_VERSION_NOT_SUPPORTED = 505,
//     VARIANT_ALSO_NEGOTIATES = 506,   // RFC 2295
//     INSUFFICIENT_STORAGE = 507,      // WebDAV
//     LOOP_DETECTED = 508,             // WebDAV
//     NOT_EXTENDED = 510,              // RFC 2774
//     NETWORK_AUTHENTICATION_REQUIRED = 511 // RFC 6585
// };
