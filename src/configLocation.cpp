
#include "../include/ConfigLocation.hpp"

ConfigLocation::ConfigLocation() {
    this->_path = "";
    this->_root = "";
    this->_autoindex = false;
}

void    ConfigLocation::setPath(const std::string& path) {
    if (path.empty() || path[0] != '/')
        throw "Invalid path";
    this->_path = path;
}

void    ConfigLocation::setRoot(const std::string& root) {
    if (ConfigParser::validRoot(root) == false)
        throw "Invalid root";
    this->_root = root;
}

void    ConfigLocation::setAutoindex(const std::string& line) {
    if (ConfigParser::validAutoindex(line) == false)
        throw "Invalid autoindex";
    if (line.find("on") != std::string::npos)
        this->_autoindex = true;
}

void    ConfigLocation::setAllowMethods(const std::string& line) {
    if (!ConfigParser::validMethods(line))
        throw "Invalid methods";
    this->_allowed_methods = splitString<std::set<std::string> >(line, ' ');
}

void    ConfigLocation::setIndex(const std::string& line) {
    this->_index = splitString<std::list<std::string> >(line, ' ');
}

void    ConfigLocation::setCgiExtension(std::list<std::string> cgi_extension) {
    //To do
    this->_cgi_extension = cgi_extension;
}

void    ConfigLocation::setCgiPath(std::string cgi_path) {
    //To do
    this->_cgi_path = cgi_path;
}

//loop through several status codes for one error page and save them in a set
void    ConfigLocation::setErrorPages(const std::string& line) {
    std::list<std::string> tmp_list = splitString<std::list<std::string> >(line, ' ');
    ErrorPage error_page;
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
    _error_pages.push_back(error_page);
}

void    ConfigLocation::setReturn(const std::string& line) {
    if (ConfigParser::validReturn(line) == false)
        throw "Invalid return";
    this->_return_value = splitString<std::list<std::string> >(line, ' ');
}

std::ostream& operator<<(std::ostream& os, const ConfigLocation& location) {
    os << "Location: " << std::endl;
    if (!location.getPath().empty()) {
        os << "\tPath: " << location.getPath() << std::endl;
    }
    if (!location.getRoot().empty()) {
        os << "\tRoot: " << location.getRoot() << std::endl;
    }
    os << "\tAutoindex: " << (location.getAutoindex() ? "on" : "off") << std::endl;

    std::set<std::string> allowed_methods = location.getAllowMethods();
    if (!allowed_methods.empty()) {
        os << "\tAllowed methods: ";
        printContainer(allowed_methods);
    }

    std::list<std::string> index = location.getIndex();
    if (!index.empty()) {
        os << "\tIndex: ";
        printContainer(index);
    }

    std::list<std::string> cgi_extension = location.getCgiExtension();
    if (!cgi_extension.empty()) {
        os << "\tCGI Extensions: ";
        printContainer(cgi_extension);
    }

    if (!location.getCgiPath().empty())
        os << "\tCGI Path: " << location.getCgiPath() << std::endl;

    std::list<ErrorPage> error_pages = location.getErrorPages();
    if (!error_pages.empty()) {
        os << "\tError pages: " << std::endl;
        for (std::list<ErrorPage>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
            os << "\tError codes: ";
            printContainer(it->error_codes);
            os << "\tError path: " << it->error_path << std::endl;
        }
    }

    std::list<std::string> return_value = location.getReturn();
    if (!return_value.empty()) {
        os << "\tReturn: ";
        printContainer(return_value);
    }
    std::cout << std::endl;
    return os;
}