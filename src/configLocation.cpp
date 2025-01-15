
#include "../include/configLocation.hpp"

configLocation::configLocation() {
    path = "";
    root = "";
    autoindex = false;
}

void    configLocation::setPath(const std::string& path) {
    struct stat buffer;
    if (path.empty() || path[0] != '/') //Checking if the path is empty or doesn't start with '/'
        throw "Invalid path";
    else if (stat(path.c_str(), &buffer) == -1 || S_ISDIR(buffer.st_mode) == 0) //Checking if the path is valid and a directory
        throw "Invalid path";
    this->path = path;
}

void    configLocation::setRoot(const std::string& root) {
    struct stat buffer;
    if (root.empty() || root[0] != '/') //Checking if the root is empty or doesn't start with '/'
        throw "Invalid root";
    else if (stat(root.c_str(), &buffer) == -1 || S_ISDIR(buffer.st_mode) == 0) //Checking if the root is valid and a directory
        throw "Invalid root";
    this->root = root;
}

void    configLocation::setAutoindex(const std::string& line) {
    //its default value is off
    if (line.find("on") != std::string::npos)
        this->autoindex = true;    
    else if (line.find("off") == std::string::npos)
        throw "Invalid autoindex value.";
}

void    configLocation::setAllowedMethods(const std::string& line) {
    std::set<std::string> tmp_methods = splitString<std::set<std::string>>(line, ' ');
    if (!configParser::validMethods(tmp_methods))
        throw "Invalid methods";
    this->allowed_methods = tmp_methods;
}

void    configLocation::setIndex(const std::string& line) {
    this->index = splitString<std::list<std::string>>(line, ' ');
}

void    configLocation::setCgiExtension(std::vector<std::string> cgi_extension) {
    //To do
    this->cgi_extension = cgi_extension;
}

void    configLocation::setCgiPath(std::string cgi_path) {
    //To do
    this->cgi_path = cgi_path;
}

//loop through several status codes for one error page and save them in a set
void    configLocation::setErrorPages(const std::string& line) {
    std::list<std::string> tmp_list = splitString<std::list<std::string>>(line, ' ');
    errorPage error_page;
    while (tmp_list.size() > 0) {
        if (onlyDigits(tmp_list.front()) && stoi(tmp_list.front()) >= 300 && stoi(tmp_list.front()) <= 599)
            error_page.error_codes.insert(stoi(tmp_list.front()));
        else if (tmp_list.front().find("/") != std::string::npos)
            error_page.error_path = tmp_list.front();
        else
            throw "Invalid error page";
        tmp_list.pop_front();
    }
    getErrorPages().push_back(error_page);
}

void    configLocation::setReturn(const std::string& line) {
    //To do
    this->return_value = line;
}

