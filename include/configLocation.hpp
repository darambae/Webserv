
#pragma once

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <vector>

struct errorPage {
    std::set<int> error_codes;
    std::string error_path;
};

class configLocation {
    private:
        std::string path;
        std::string root;
        bool autoindex;
        std::set<std::string> allowed_methods; 
        std::list<std::string> index;
        std::vector<std::string> cgi_extension;
        std::string cgi_path;
        std::list<errorPage> error_pages;
        int limit_client_body_size;

    public:
        configLocation();
        ~configLocation() {};

        std::string getPath() const { return path; };
        std::string getRoot() const { return root; };
        bool getAutoindex() const { return autoindex; };
        std::set<std::string> getAllowedMethods() const { return allowed_methods; };
        std::list<std::string> getIndex() const { return index; };
        std::vector<std::string> getCgiExtension() const { return cgi_extension; };
        std::string getCgiPath() const { return cgi_path; };
        std::list<errorPage> getErrorPages() const { return error_pages; };
        int getLimitClientBodySize() const { return limit_client_body_size; };

        void setPath(std::string path);
        void setRoot(std::string root);
        void setAutoindex(bool autoindex);
        void setAllowedMethods(std::set<std::string> allowed_methods);
        void setIndex(std::list<std::string> index);
        void setCgiExtension(std::vector<std::string> cgi_extension);
        void setCgiPath(std::string cgi_path);
        void setErrorPages(std::list<errorPage> error_pages);
        void setLimitClientBodySize(int limit_client_body_size);





};