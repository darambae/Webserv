#pragma once

#include <iostream>
#include <list>
#include <set>
#include <vector>
#include "ConfigLocation.hpp"
#include "ConfigParser.hpp"
#include <cstring> //memset...
#include <cstdlib> //exit ...
#include <sys/socket.h> // socket, bind, accept...
#include <netinet/in.h> // sockaddr_in
#include <unistd.h> //close...
#include <poll.h>
#include <algorithm>
#include <map>
#include "Request.hpp"

struct errorPage {
    std::set<int> error_codes;
    std::string error_path;
};

class ConfigLocation;

class ConfigServer {
    private:
		static	std::map<int, int>	mapPortFd;
        std::string _ip;
        std::vector<int> _port;
        std::vector<std::string> _server_names;
        std::string _root;
        unsigned long _limit_client_body_size; //ex)10, 10k, 10m (didn't consider g)
        std::list<errorPage> _error_pages;
        std::vector<ConfigLocation> _locations;
		struct sockaddr_in	_address;
		int _client_count, _len_address, _max_clients, _indexServer, _indexClient;
		//indexServer = in fds between index = 0 and index = indexServer, all fd are serverFd
		//indexClient = in fds since this index, all fd are for client
		std::vector<struct pollfd>	_fds;//stock all fd (server + client)
		Request	_request;

    public:
        ConfigServer();
        ~ConfigServer() {};

        std::string getIp() const { return _ip; };
        int getPort() const { return _port; };
        std::vector<std::string> getServerNames() const { return _server_names; };
        std::string getRoot() const { return _root; };
        int getLimitClientBodySize() const { return _limit_client_body_size; };
        std::list<errorPage> getErrorPages() const { return _error_pages; };
        std::vector<ConfigLocation> getLocations() const { return _locations; };

        void setIp(const std::string& ip);
        void setPort(const std::string& port);
        void setServerNames(const std::string& server_name);
        void setRoot(const std::string& root);
        void setLimitClientBodySize(const std::string& limit_client_body_size);
        void setErrorPages(const std::string& error_page);
        void setLocations(const ConfigLocation& location);

		std::vector<int> &	createServerFd();
		class ServerException : public std::exception {
		private:
		std::string	_message;
		public:
		ServerException(std::string const & message) : _message(message) {}
		virtual ~ServerException() throw() {}
		virtual const char* what() const throw() {return _message.c_str();}
	};
};

std::ostream& operator<<(std::ostream& os, const ConfigServer& c);
