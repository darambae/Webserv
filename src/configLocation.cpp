
#include "../include/ConfigLocation.hpp"

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

void    ConfigLocation::setAllowMethods(const std::string& line) {
    if (!ConfigParser::validMethods(line))
        throw "Invalid methods";
    this->allowed_methods = splitString<std::set<std::string> >(line, ' ');
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
    error_pages.push_back(error_page);
}

void    ConfigLocation::setReturn(const std::string& line) {
    if (ConfigParser::validReturn(line) == false)
        throw "Invalid return";
    this->return_value = splitString<std::list<std::string> >(line, ' ');
}

std::ostream& operator<<(std::ostream& os, const ConfigLocation& location) {
    os << "Location: " << std::endl;
    if (!location.getPath().empty()) {
        os << "Path: " << location.getPath() << std::endl;
    }
    if (!location.getRoot().empty()) {
        os << "Root: " << location.getRoot() << std::endl;
    }
    os << "Autoindex: " << (location.getAutoindex() ? "on" : "off") << std::endl;

    std::set<std::string> allowed_methods = location.getAllowMethods();
    if (!allowed_methods.empty()) {
        os << "Allowed methods: ";
        for (std::set<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
            os << *it << " ";
        os << std::endl;
    }

    std::list<std::string> index = location.getIndex();
    if (!index.empty()) {
        os << "Index: ";
        for (std::list<std::string>::const_iterator it = index.begin(); it != index.end(); ++it)
            os << *it << " ";
        os << std::endl;
    }

    std::vector<std::string> cgi_extension = location.getCgiExtension();
    if (!cgi_extension.empty()) {
        os << "CGI Extensions: ";
        for (std::vector<std::string>::const_iterator it = cgi_extension.begin(); it != cgi_extension.end(); ++it)
            os << *it << " ";
        os << std::endl;
    }

    if (!location.getCgiPath().empty()) {
        os << "CGI Path: " << location.getCgiPath() << std::endl;
    }

    std::list<ErrorPage> error_pages = location.getErrorPages();
    if (!error_pages.empty()) {
        os << "Error pages: " << std::endl;
        for (std::list<ErrorPage>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
            os << "Error codes: ";
            for (std::set<int>::const_iterator it_code = it->error_codes.begin(); it_code != it->error_codes.end(); ++it_code)
                os << *it_code << " ";
            os << std::endl;
            os << "Error path: " << it->error_path << std::endl;
        }
    }

    std::list<std::string> return_value = location.getReturn();
    if (!return_value.empty()) {
        os << "Return: ";
        for (std::list<std::string>::const_iterator it = return_value.begin(); it != return_value.end(); ++it)
            os << *it << " ";
        os << std::endl;
    }

    return os;
}
