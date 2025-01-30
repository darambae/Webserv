/*By parsing configuration file following the rule of nginx config file, 
save the data in a struct and return a vector of servers info.*/

#include "../include/ConfigParser.hpp"


ConfigParser::ConfigParser(const std::string& file) {
    this->setFilePath(file);
}

void    ConfigParser::setFilePath(const std::string& file) {
    std::ifstream   test(file.c_str());
    if (!test.is_open())
        THROW("Configuration file cannot be opened.");
    this->_filePath = file;
}

void    ConfigParser::setServers(const ConfigServer& server) {
    this->_servers.push_back(server);
}

void    ConfigParser::parseFile() {
    ConfigServer server;
    std::string line;
    std::ifstream file(this->getFilePath().c_str());

    if (!file.is_open())
        THROW("Configuration file cannot be opened.");

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') { continue; }
        if (line.find("server") != std::string::npos && validBracket(line, '{', '}')) {
            parseDirectives(file, server);
            setServers(server);
        }
    }
    if (file.is_open())
        file.close();
}

void    ConfigParser::parseDirectives(std::ifstream &file, ConfigServer &server) {
    std::string line;
    while (getline(file, line)) {
        removeWhitespaces(line);
        size_t start_pos = line.find(" ") + 1;
        size_t end_pos = line.find(";");
        size_t len = end_pos - start_pos; //string length from space to semicolon
        if (validBracket(line, '}', '{')) { break; }
        if (line.empty() || line[0] == '#') { continue; }
        if (line.find("listen ") != std::string::npos && endingSemicolon(line)) {
            if (server.getListen().size() == 1 &&
                server.getListen().front().first == "0.0.0.0" &&
                server.getListen().front().second == 8080)
                server.getListen().clear(); //remove default listen
            if (line.find(":") != std::string::npos)
                server.setListen(line.substr(start_pos, line.find(":") - start_pos), 
                    line.substr(line.find(":") + 1, end_pos - line.find(":") - 1));
            else {
                if (line.find(".") != std::string::npos)
                    server.setListen(line.substr(start_pos, len), "8080");
                else
                    server.setListen("0.0.0.0" ,line.substr(start_pos, len));
            }
        }
        else if (line.find("server_name ") != std::string::npos && endingSemicolon(line)) {
            const std::string& server_name = line.substr(start_pos, len);
            server.setServerNames(server_name);
        }
        else if (line.find("root ") != std::string::npos && endingSemicolon(line))
            server.setRoot(line.substr(start_pos, len));
        else if (line.find("client_max_body_size ") != std::string::npos && endingSemicolon(line))
            server.setLimitClientBodySize(line.substr(start_pos, len));
        else if (line.find("error_page ") != std::string::npos && endingSemicolon(line))
            server.setErrorPages(line.substr(start_pos, len));
        else if (line.find("location ") != std::string::npos && validBracket(line, '{', '}')) {
            ConfigLocation location;
            parseLocation(file, line, location);
            server.setLocations(location);
        }
    }
}

void    ConfigParser::parseLocation(std::ifstream &file, std::string line, ConfigLocation &location) {
    std::string path = line.substr(line.find(" ") + 1, line.find(" {") - line.find(" ") - 1);
    location.setPath(path);
    while (getline(file, line)) {
        removeWhitespaces(line);
        size_t start_pos = line.find(" ") + 1;
        size_t end_pos = line.find(";");
        size_t len = end_pos - start_pos;
        std::vector<ErrorPage> &error_pages = location.getErrorPages();
        if (validBracket(line, '}', '{'))
            break;
        if (line.empty() || line[0] == '#') { continue; }
        if (line.find("root ") != std::string::npos && endingSemicolon(line))
            location.setRoot(line.substr(start_pos, len));
        else if (line.find("autoindex ") != std::string::npos && endingSemicolon(line))
            location.setAutoindex(line.substr(start_pos, len));
        else if (line.find("index ") != std::string::npos && endingSemicolon(line))
            location.setIndex(line.substr(start_pos, len));
        else if (line.find("allow_methods ") != std::string::npos && endingSemicolon(line))
            location.setAllowMethods(line.substr(start_pos, len));
        else if (line.find("return ") != std::string::npos && endingSemicolon(line))
            location.setReturn(line.substr(start_pos, len));
        else if (line.find("error_page ") != std::string::npos && endingSemicolon(line)) {
            if (error_pages.size() == 1 && error_pages.front().error_codes.count(404) && error_pages.front().error_path == "/data/www/errors/404.html")
                error_pages.clear();
            location.setErrorPages(line.substr(start_pos, len));
        }
    }
}

void    ConfigParser::validMethods(const std::string& methods) {
    std::vector<std::string> tmp_vector = splitString<std::vector<std::string> >(methods, ' ');
    while (!tmp_vector.empty()) {
        const std::string& token = tmp_vector.front();
        if (token != "GET" && token != "POST" && token != "DELETE" && token != "PUT")
            THROW("Invalid methods");
        tmp_vector.erase(tmp_vector.begin());
    }
    //Logger::getInstance(CONSOLE_OUTPUT).log(INFO, "Valid methods");
}

void    ConfigParser::validIp(std::string ip) {
    std::string  segment;
    size_t  pos;
    int     num;
    
    if (ip == "localhost")
        Logger::getInstance(CONSOLE_OUTPUT).log(INFO, "");
    while ((pos = ip.find(".")) != std::string::npos) {
        segment = ip.substr(0, pos);
        num = std::atoi(segment.c_str());
        if (num < 0 || num > 255)
            THROW("The given IP is Out of range");
        ip = ip.substr(pos + 1);
    }
    num = std::atoi(ip.c_str());
    if (num < 0 || num > 255)
        THROW("The given IP is Out of range");
    //Logger::getInstance(CONSOLE_OUTPUT).log(INFO, "Valid ip");
}

void    ConfigParser::validPort(const std::string& port) {
    int num = std::atoi(port.c_str());
    if (num < 0 || num > 65535)
        THROW("Invalid Port");
    //Logger::getInstance(CONSOLE_OUTPUT).log(INFO, "Valid port");
}

void    ConfigParser::validAutoindex(const std::string& line) {
    if (line.find("on") != std::string::npos || line.find("off") != std::string::npos)
        //Logger::getInstance(CONSOLE_OUTPUT).log(INFO, "Valid autoindex");
        return;
    THROW("Invalid autoindex");
}

void    ConfigParser::validErrorPage(const std::string& line) {
    std::vector<std::string> tmp_vector = splitString<std::vector<std::string> >(line, ' ');
    while (!tmp_vector.empty()) {
        const std::string& token = tmp_vector.front();
        if (onlyDigits(token) && std::atoi(token.c_str()) >= 300 && std::atoi(token.c_str()) <= 599) {
            tmp_vector.erase(tmp_vector.begin());
            continue;
        }
        if (token[0] == '/' && tmp_vector.size() == 1)
            //Logger::getInstance(CONSOLE_OUTPUT).log(INFO, "Valid error page");
            return;
        tmp_vector.erase(tmp_vector.begin());
    }
    THROW("Invalid error page");
}

void    ConfigParser::validReturn(const std::string& line) {
    std::vector<std::string> tmp_vector = splitString<std::vector<std::string> >(line, ' ');
    if (tmp_vector.size() > 2 || tmp_vector.size() == 0)
        THROW("Invalid return format");
    if (tmp_vector.size() == 1) {
        const std::string& value = tmp_vector.front();
        if (onlyDigits(value) && std::atoi(value.c_str()) >= 300 && atoi(value.c_str()) <= 599)
        {
            //Logger::getInstance(CONSOLE_OUTPUT).log(INFO, "Valid return");
            return;
        }
        THROW("Invalid return");
    }
    if (tmp_vector.size() == 2) {
        const std::string&  status_code = tmp_vector.front();
        const std::string&  path = tmp_vector.back();
        if (!onlyDigits(status_code) || std::atoi(status_code.c_str()) < 300 || std::atoi(status_code.c_str()) > 599)
            THROW("Invalid return status code");
        validRoot(path);
        // if (path.find("https://") == std::string::npos && path.find("http://") == std::string::npos)
        //     THROW("Invalid return path");
        //Logger::getInstance(CONSOLE_OUTPUT).log(INFO, "Valid return");
        return;
    }   
    THROW("Invalid return");
}

void    ConfigParser::validRoot(const std::string& line) {
    struct stat buffer;
    char resolved_path[100]; //possible memory leak
    std::string path = line.length() > 1 ? line.substr(1) : line;

    if (line.empty() || line[0] != '/')
        THROW ("Invalid root");
    if (realpath(path.c_str(), resolved_path) == NULL)
        THROW ("Realpath failed");
    if (stat(resolved_path, &buffer) == -1 || S_ISDIR(buffer.st_mode) == 0)
        THROW ("Invalid root");
    //Logger::getInstance(CONSOLE_OUTPUT).log(INFO, "Valid root");
}

void    ConfigParser::validBodySize(const std::string& line) {
    unsigned long num = std::strtoul(line.c_str(), NULL, 10);
    if (num < 0 || std::isalpha(line[line.size() - 2]))
        THROW("Invalid body size");
    if (onlyDigits(line) || line[line.size() - 1] == 'k' || line[line.size() - 1] == 'K' ||
            line[line.size() - 1] == 'm' || line[line.size() - 1] == 'M')
        //Logger::getInstance(CONSOLE_OUTPUT).log(INFO, "Valid body size");
        return;
    THROW("Invalid body size format");
}

void    ConfigParser::validCgiExtension(const std::string& line) {
    std::vector<std::string> tmp_vector = splitString<std::vector<std::string> >(line, ' ');
    while (!tmp_vector.empty()) {
        const std::string& token = tmp_vector.front();
        if (token != ".py" && token != ".php" && token != ".cgi")
            THROW("Invalid CGI extension");
        tmp_vector.erase(tmp_vector.begin());
    }
    //Logger::getInstance(CONSOLE_OUTPUT).log(INFO, "Valid CGI extension");
}
