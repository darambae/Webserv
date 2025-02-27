
#include "../include/ConfigLocation.hpp"

void    ConfigLocation::setPath(const std::string& path) {
    if (path.empty() || path[0] != '/')
        THROW("Invalid path");
    LOG_DEBUG("Valid path");
    this->_path = path;
}

void    ConfigLocation::setRoot(const std::string& root) {
    ConfigParser::validRoot(root);
    LOG_DEBUG("Valid root");
    this->_root = root;
}

void    ConfigLocation::setAutoindex(const std::string& line) {
    ConfigParser::validAutoindex(line);
    LOG_DEBUG("Valid autoindex");
    if (line.find("on") != std::string::npos)
        this->_autoindex = true;
}

void    ConfigLocation::setAllowMethods(const std::string& line) {
    ConfigParser::validMethods(line);
    LOG_DEBUG("Valid methods");
    this->_allowed_methods = splitString<std::set<std::string> >(line, ' ');
}

void    ConfigLocation::setIndex(const std::string& line) {
    this->_index = splitString<std::vector<std::string> >(line, ' ');
}

void    ConfigLocation::setCgiExtension(const std::string& line) {
    ConfigParser::validCgiExtension(line);
    LOG_DEBUG("Valid CGI extension");
    this->_cgi_extension =  splitString<std::vector<std::string> >(line, ' ');
}

void    ConfigLocation::setCgiPass(const std::string& line) {
    std::vector<std::string> tmp_vector = splitString<std::vector<std::string> >(line, ' ');
    while (!tmp_vector.empty()) {
        if (access(tmp_vector.front().c_str(), X_OK) == -1)
            THROW("Invalid CGI pass");
        this->_cgi_pass.push_back(tmp_vector.front());
        tmp_vector.erase(tmp_vector.begin());
    }
    LOG_DEBUG("Valid CGI path");
}

//loop through several status codes for one error page and save them in a set
void    ConfigLocation::setErrorPages(const std::string& line) {
    std::vector<std::string> tmp_vector = splitString<std::vector<std::string> >(line, ' ');
    ErrorPage error_page;
    ConfigParser::validErrorPage(line);
    LOG_DEBUG("Valid error page");
    while (!tmp_vector.empty()) {
        const std::string& token = tmp_vector.front();
        if (token[0] == '/')
            error_page.error_path = token;
        else
            error_page.error_codes.insert(atoi(token.c_str()));
        tmp_vector.erase(tmp_vector.begin());
    }
    _error_pages.push_back(error_page);
}

void    ConfigLocation::setReturn(const std::string& line) {
    ConfigParser::validReturn(line);
    this->_return_value = splitString<std::vector<std::string> >(line, ' ');
}

std::ostream& operator<<(std::ostream& os, const ConfigLocation& location) {
    os << "Location: " << std::endl;
    if (!location.getPath().empty())
        os << "\tPath: " << location.getPath() << std::endl;
    if (!location.getRoot().empty())
        os << "\tRoot: " << location.getRoot() << std::endl;
    os << "\tAutoindex: " << (location.getAutoindex() ? "on" : "off") << std::endl;

    std::set<std::string> allowed_methods = location.getAllowMethods();
    if (!allowed_methods.empty()) {
        os << "\tAllowed methods: ";
        printContainer(allowed_methods);
    }

    std::vector<std::string> index = location.getIndex();
    if (!index.empty()) {
        os << "\tIndex: ";
        printContainer(index);
    }

    std::vector<std::string> cgi_extension = location.getCgiExtension();
    if (!cgi_extension.empty()) {
        os << "\tCGI Extensions: ";
        printContainer(cgi_extension);
    }

    if (!location.getCgiPass().empty()) {
        std::vector<std::string> cgi_pass = location.getCgiPass();
        os << "\tCGI Pass: ";
        printContainer(cgi_pass);
    }

    std::vector<ErrorPage> error_pages = location.getErrorPages();
    if (!error_pages.empty()) {
        os << "\tError pages: " << std::endl;
        for (std::vector<ErrorPage>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
            os << "\tError codes: ";
            printContainer(it->error_codes);
            os << "\tError path: " << it->error_path << std::endl;
        }
    }

    std::vector<std::string> return_value = location.getReturn();
    if (!return_value.empty()) {
        os << "\tReturn: ";
        printContainer(return_value);
    }
    std::cout << std::endl;
    return os;
}