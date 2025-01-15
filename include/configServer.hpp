#pragma once

#include <iostream>
#include <list>
#include <set>
#include <vector>
#include "configLocation.hpp"
#include "configParser.hpp"

struct errorPage {
    std::set<int> error_codes;
    std::string error_path;
};

class configServer {
    private:
        std::string ip;
        int port;
        std::vector<std::string> server_names;
        std::string root;
        int limit_client_body_size;
        std::list<errorPage> error_pages;
        std::vector<configLocation> locations;
    
    public:
        configServer();
        ~configServer() {};

        std::string getIp() const { return ip; };
        int getPort() const { return port; };
        std::vector<std::string> getServerNames() const { return server_names; };
        std::string getRoot()  { return root; };
        int getLimitClientBodySize() const { return limit_client_body_size; };
        std::list<errorPage> getErrorPages() const { return error_pages; };
        std::vector<configLocation> getLocations() const { return locations; };

        void setIp(const std::string& ip);
        void setPort(const std::string& port);
        void setServerNames(const std::string& server_name);
        void setRoot(const std::string& root);
        void setLimitClientBodySize(const std::string& limit_client_body_size);
        void setErrorPages(const std::string& error_page);
        void setLocations(configLocation location);
};