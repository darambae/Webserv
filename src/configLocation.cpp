
#include "../include/ConfigLocation.hpp"
#include "../include/ConfigParser.hpp"

ConfigLocation::ConfigLocation() {
    path = "";
    root = "";
    autoindex = false;
}

void    ConfigLocation::setPath(const std::string& path) {
    if (ConfigParser::validRoot(path) == false)
        throw "Invalid path";
    this->path = path;
}

void    ConfigLocation::setRoot(const std::string& root) {
    if (ConfigParser::validRoot(root) == false)
        throw "Invalid root";
    this->root = root;
}

void    ConfigLocation::setAutoindex(const std::string& line) {
    if (ConfigParser::validAutoindex(line) == false)
        throw "Invalid autoindex";
    if (line.find("on") != std::string::npos)
        this->autoindex = true;
}

void    ConfigLocation::setAllowedMethods(const std::string& line) {
    std::set<std::string> tmp_methods = splitString<std::set<std::string> >(line, ' ');
    if (!ConfigParser::validMethods(tmp_methods))
        throw "Invalid methods";
    this->allowed_methods = tmp_methods;
}

void    ConfigLocation::setIndex(const std::string& line) {
    this->index = splitString<std::list<std::string> >(line, ' ');
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
    std::list<std::string> tmp_list = splitString<std::list<std::string> >(line, ' ');
    errorPage error_page;
    if (ConfigParser::validErrorPage(line) == false)
        throw "Invalid error page";
    while (!tmp_list.empty()) {
        const std::string& token = tmp_list.front();
        if (token[0] == '/')
            error_page.error_path = token;
        else
            error_page.error_codes.insert(atoi(token.c_str()));
        tmp_list.pop_front();
    }
    error_pages.push_back(error_page);
}

void    ConfigLocation::setReturn(const std::string& line) {
    if (ConfigParser::validReturn(line) == false)
        throw "Invalid return";
    this->return_value = splitString<std::list<std::string> >(line, ' ');
}

std::ostream& operator<<(std::ostream& os, const ConfigLocation& location) {
    os << "Location path: " << location.getPath() << std::endl;
    os << "Root: " << location.getRoot() << std::endl;
    os << "Auto_index: " << location.getAutoindex() << std::endl;
    return os;
}
