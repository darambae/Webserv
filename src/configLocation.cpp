
#include "../include/ConfigLocation.hpp"
#include "../include/ConfigParser.hpp"

ConfigLocation::ConfigLocation() {
    path = "";
    root = "";
    autoindex = false;
}

void    ConfigLocation::setPath(const std::string& path) {
    struct stat buffer;
    if (path.empty() || path[0] != '/') //Checking if the path is empty or doesn't start with '/'
        throw "Invalid path";
    else if (stat(path.c_str(), &buffer) == -1 || S_ISDIR(buffer.st_mode) == 0) //Checking if the path is valid and a directory
        throw "Invalid path";
    this->path = path;
}

void    ConfigLocation::setRoot(const std::string& root) {
    struct stat buffer;
    if (root.empty() || root[0] != '/') //Checking if the root is empty or doesn't start with '/'
        throw "Invalid root";
    else if (stat(root.c_str(), &buffer) == -1 || S_ISDIR(buffer.st_mode) == 0) //Checking if the root is valid and a directory
        throw "Invalid root";
    this->root = root;
}

void    ConfigLocation::setAutoindex(const std::string& line) {
    //its default value is off
    if (line.find("on") != std::string::npos)
        this->autoindex = true;    
    else if (line.find("off") == std::string::npos)
        throw "Invalid autoindex value.";
}

void    ConfigLocation::setAllowedMethods(const std::string& line) {
    std::set<std::string> tmp_methods = splitString<std::set<std::string>>(line, ' ');
    if (!ConfigParser::validMethods(tmp_methods))
        throw "Invalid methods";
    this->allowed_methods = tmp_methods;
}

void    ConfigLocation::setIndex(const std::string& line) {
    this->index = splitString<std::list<std::string>>(line, ' ');
}

void    ConfigLocation::setCgiExtension(std::vector<std::string> cgi_extension) {
    //To do
    this->cgi_extension = cgi_extension;
}

void    ConfigLocation::setCgiPath(std::string cgi_path) {
    //To do
    this->cgi_path = cgi_path;
}

//loop through several status codes for one error page and save them in a set
void    ConfigLocation::setErrorPages(const std::string& line) {
    std::list<std::string> tmp_list = splitString<std::list<std::string>>(line, ' ');
    errorPage error_page;
    if (ConfigPasrser::validErrorPage(line) == false)
        throw "Invalid error page";
    while (!tmp_list.empty()) {
        const std::string& token = tmp_list.front();
        if (token[0] == '/')
            error_page.error_path = token;
        else
            error_page.error_codes.insert(stoi(token));
        tmp_list.pop_front();
    }
    getErrorPages().push_back(error_page);
}

void    ConfigLocation::setReturn(const std::string& line) {
    if (ConfigParser::validReturn(line) == false)
        throw "Invalid return";
    this->return_value = splitString<std::list<std::string>>(line, ' ');
}

