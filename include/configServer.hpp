#pragma once

#include <iostream>
#include <list>
#include <set>
#include <vector>
#include "../include/configLocation.hpp"

struct errorPage {
    std::set<int> error_codes;
    std::string error_path;
};

// struct to store the configuration of one server
class configServer {
    private:
        std::string ip;
        int port;
        std::vector<std::string> server_names;
        std::string root;
        int limit_client_body_size;
        std::list<errorPage> default_error_pages;
        std::vector<configLocation> locations;
    
    public:
        configServer();
        ~configServer() {};

        std::string getIp() const { return ip; };
        int getPort() const { return port; };
        std::vector<std::string> getServerNames() const { return server_names; };
        std::string getRoot()  { return root; };
        int getLimitClientBodySize() const { return limit_client_body_size; };
        std::list<errorPage> getDefaultErrorPages() const { return default_error_pages; };
        std::vector<configLocation> getLocations() const { return locations; };

        void setIp(std::string ip);
        void setPort(int port);
        void setServerNames(std::vector<std::string> server_names);
        void setRoot(std::string root);
        void setLimitClientBodySize(int limit_client_body_size);
        void setDefaultErrorPages(std::list<errorPage> default_error_pages);
        void setLocations(std::vector<configLocation> locations);

        bool validIp(std::string ip);
};