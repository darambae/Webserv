
#pragma once

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <vector>
#include "sys/stat.h"

struct errorPage {
    std::set<int> error_codes;
    std::string error_path;
};

class ConfigLocation {
    private:
        std::string path;
        std::string root;
        bool autoindex;
        std::set<std::string> allowed_methods; 
        std::list<std::string> index;
        std::vector<std::string> cgi_extension;
        std::string cgi_path;
        std::list<errorPage> error_pages;
        std::list<std::string> return_value;
    public:
        ConfigLocation();
        ~ConfigLocation() {};

        std::string getPath() const { return path; };
        std::string getRoot() const { return root; };
        bool getAutoindex() const { return autoindex; };
        std::set<std::string> getAllowedMethods() const { return allowed_methods; };
        std::list<std::string> getIndex() const { return index; };
        std::vector<std::string> getCgiExtension() const { return cgi_extension; };
        std::string getCgiPath() const { return cgi_path; };
        std::list<errorPage> getErrorPages() const { return error_pages; };

        void setPath(const std::string& path);
        void setRoot(const std::string& root);
        void setAutoindex(const std::string& line);
        void setAllowedMethods(const std::string& line);
        void setIndex(const std::string& line);
        void setCgiExtension(std::vector<std::string> cgi_extension);
        void setCgiPath(std::string cgi_path);
        void setErrorPages(const std::string& line);
        void setReturn(const std::string& line);

};