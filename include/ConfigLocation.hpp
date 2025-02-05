
#pragma once

#include "webserv.hpp"

struct ErrorPage;

class ConfigLocation {
    private:
        std::string _path;
        std::string _root;
        bool _autoindex;
        std::set<std::string> _allowed_methods;
        std::vector<std::string> _index;
        std::vector<std::string> _cgi_extension;
        std::string _cgi_path;
        std::vector<ErrorPage> _error_pages;
        std::vector<std::string> _return_value;
    public:
        ConfigLocation();
        ~ConfigLocation() {};

        std::string getPath() const { return _path; };
        std::string getRoot() const { return _root; };
        bool getAutoindex() const { return _autoindex; };
        std::set<std::string> getAllowMethods() const { return _allowed_methods; };
        std::vector<std::string> getIndex() const { return _index; };
        std::vector<std::string> getCgiExtension() const { return _cgi_extension; };
        std::string getCgiPath() const { return _cgi_path; };
        std::vector<ErrorPage> getErrorPages() const { return _error_pages; };
        std::vector<std::string> getReturn() const { return _return_value; };

        void setPath(const std::string& path);
        void setRoot(const std::string& root);
        void setAutoindex(const std::string& line);
        void setAllowMethods(const std::string& line);
        void setIndex(const std::string& line);
        void setCgiExtension(const std::string& line);
        void setCgiPath(const std::string& line);
        void setErrorPages(const std::string& line);
        void setDefaultErrorPages();
        void setReturn(const std::string& line);
};

std::ostream& operator<<(std::ostream& os, const ConfigLocation& c);
