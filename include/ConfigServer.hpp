#pragma once

#include <iostream>
#include <list>
#include <set>
#include "ConfigLocation.hpp"
#include "ConfigParser.hpp"
#include "webserv.hpp"

class ConfigLocation;

class ConfigServer {
    private:
        std::string _ip;
        int _port;
        std::list<std::string> _server_names;
        std::string _root;
        unsigned long _limit_client_body_size; //ex)10, 10k, 10m (didn't consider g)
        std::list<ErrorPage> _error_pages;
        std::list<ConfigLocation> _locations;

    public:
        ConfigServer();
        ~ConfigServer() {};

        std::string getIp() const { return _ip; };
        int getPort() const { return _port; };
        std::list<std::string> getServerNames() const { return _server_names; };
        std::string getRoot() const { return _root; };
        int getLimitClientBodySize() const { return _limit_client_body_size; };
        std::list<ErrorPage> getErrorPages() const { return _error_pages; };
        std::list<ConfigLocation> getLocations() const { return _locations; };

        void setIp(const std::string& ip);
        void setPort(const std::string& port);
        void setServerNames(const std::string& server_name);
        void setRoot(const std::string& root);
        void setLimitClientBodySize(const std::string& limit_client_body_size);
        void setErrorPages(const std::string& error_page);
        void setLocations(const ConfigLocation& location);
};;

std::ostream& operator<<(std::ostream& os, const ConfigServer& c);
