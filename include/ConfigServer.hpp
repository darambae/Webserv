#pragma once

#include "webserv.hpp"

class ConfigLocation;
struct ErrorPage;

class ConfigServer {
    private:

        std::vector<std::pair<std::string, int> > _listen;
        std::vector<std::string> _server_names;
        std::vector<std::string> _index;
        std::string _root;
        unsigned long _limit_client_body_size; // ex)10, 10k, 10m (didn't consider g)
        std::vector<ErrorPage> _error_pages;
        std::vector<ConfigLocation> _locations;

    public:
    
        ConfigServer() : _limit_client_body_size(-1) {};
        ~ConfigServer() {};

        std::vector<std::pair<std::string, int> > getListen() const { return _listen; };
        std::vector<std::string>                  getServerNames() const { return _server_names; };
        std::string                               getRoot() const { return _root; };
        int                                       getLimitClientBodySize() const { return _limit_client_body_size; };
        std::vector<ErrorPage>                    getErrorPages() const { return _error_pages; };
        std::vector<ConfigLocation> const&        getLocations() const { return _locations; };
        std::vector<std::string> const& getIndex() const { return _index; };

        void setListen(const std::string &ip, const std::string &port);
        void setDefaultListen();
        void setServerNames(const std::string &server_name);
        void setIndex(const std::string& line);
        void setRoot(const std::string &root);
        void setLimitClientBodySize(const std::string &limit_client_body_size);
        void setErrorPages(const std::string &error_page);
        void setLocations(const ConfigLocation &location);
};
;

std::ostream& operator<<(std::ostream& os, const ConfigServer& c);
