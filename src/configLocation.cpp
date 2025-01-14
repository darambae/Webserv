
#include "../include/configLocation.hpp"

configLocation::configLocation() {
    path = "";
    root = "";
    autoindex = false;
}

void    configLocation::setPath(std::string path) {
    //To do
    this->path = path;
}

void    configLocation::setRoot(std::string root) {
    //To do
    this->root = root;
}

void    configLocation::setAutoindex(bool autoindex) {
    //To do
    this->autoindex = autoindex;
}

void    configLocation::setAllowedMethods(std::set<std::string> allowed_methods) {
    //To do
    this->allowed_methods = allowed_methods;
}

void    configLocation::setIndex(std::list<std::string> index) {
    //To do
    this->index = index;
}

void    configLocation::setCgiExtension(std::vector<std::string> cgi_extension) {
    //To do
    this->cgi_extension = cgi_extension;
}

void    configLocation::setCgiPath(std::string cgi_path) {
    //To do
    this->cgi_path = cgi_path;
}

void    configLocation::setErrorPages(std::list<errorPage> error_pages) {
    //To do
    this->error_pages = error_pages;
}



