#pragma once

#include <iostream>
#include <list>
#include <set>
#include <vector>
#include "ConfigLocation.hpp"
#include "ConfigParser.hpp"

struct errorPage {
    std::set<int> error_codes;
    std::string error_path;
};

class ConfigServer {
    private:
        std::string ip;
        int port;
        std::vector<std::string> server_names;
        std::string root;
        unsigned long limit_client_body_size; //ex)10, 10k, 10m (didn't consider g)
        std::list<errorPage> error_pages;
        std::vector<ConfigLocation> locations;
    
    public:
        ConfigServer();
        ~ConfigServer() {};

        std::string getIp() const { return ip; };
        int getPort() const { return port; };
        std::vector<std::string> getServerNames() const { return server_names; };
        std::string getRoot()  { return root; };
        int getLimitClientBodySize() const { return limit_client_body_size; };
        std::list<errorPage> getErrorPages() const { return error_pages; };
        std::vector<ConfigLocation> getLocations() const { return locations; };

        void setIp(const std::string& ip);
        void setPort(const std::string& port);
        void setServerNames(const std::string& server_name);
        void setRoot(const std::string& root);
        void setLimitClientBodySize(const std::string& limit_client_body_size);
        void setErrorPages(const std::string& error_page);
        void setLocations(const ConfigLocation& location);
};
