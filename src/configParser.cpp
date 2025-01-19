/*By parsing configuration file following the rule of nginx config file, 
save the data in a struct and return a list of servers info.*/

#include "../include/ConfigParser.hpp"

/*Pseudo code
1. Read the configuration file line by line.
2. Parse the file.
3. Initailize a list of configServer.
4. Check format of the configuration file.
5. Check validity of the configuration file.
6. Save the data in a struct and return it.
*/

ConfigParser::ConfigParser(const std::string& file) {
    this->setFilePath(file);
}

void    ConfigParser::setFilePath(const std::string& file) {
    std::ifstream   test(file);
    if (!test.is_open())
        throw "Configuration file cannot be opened.";
    this->filePath = file;
}

void    ConfigParser::setServers(const ConfigServer& server) {
    this->servers.push_back(server);
}

void    ConfigParser::parseFile() {
    ConfigServer server;
    std::string line;
    std::ifstream file(this->getFilePath());

    if (!file.is_open())
        throw "Configuration file cannot be opened.";

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') { continue; }
        if (line.find("server {") != std::string::npos) {
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
        if (line.find("}") != std::string::npos) { break; }
        if (line.empty() || line[0] == '#') { continue; }
        if (line.find("listen ") != std::string::npos && endingSemicolon(line)) {
            if (line.find(":") != std::string::npos) {
                server.setIp(line.substr(start_pos, line.find(":") - start_pos));
                server.setPort(line.substr(line.find(":") + 1, end_pos - line.find(":") - 1));
            }
            else {
                if (line.find(".") != std::string::npos)
                    server.setIp(line.substr(start_pos, len));
                else
                    server.setPort(line.substr(start_pos, len));
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
        else if (line.find("error_page ") != std::string::npos && endingSemicolon(line)) {
            server.setErrorPages(line.substr(start_pos, len));
        }
        else if (line.find("location ") != std::string::npos) {
            ConfigLocation location;
            parseLocation(file, line, location);
            server.setLocations(location);
        }
    }
}

void    ConfigParser::parseLocation(std::ifstream &file, std::string line, ConfigLocation &location) {
    std::string path = line.substr(line.find(" ") + 1, line.find("{") - line.find(" ") - 1);
    location.setPath(path);
    while (getline(file, line)) {
        removeWhitespaces(line);
        size_t start_pos = line.find(" ") + 1;
        size_t end_pos = line.find(";");
        size_t len = end_pos - start_pos;
        if (line.find("}") != std::string::npos) { break; }
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
        
    }
}

bool    ConfigParser::validMethods(const std::string& methods) {
    std::list<std::string> tmp_list = splitString<std::list<std::string> >(methods, ' ');
    while (!tmp_list.empty()) {
        const std::string& token = tmp_list.front();
        if (token != "GET" && token != "POST" && token != "DELETE" && token != "PUT")
            return false;
        tmp_list.pop_front();
    }
    return true;
}

bool    ConfigParser::validIp(std::string ip) {
    std::string  segment;
    size_t  pos;
    int     num;
    
    if (ip == "localhost")
        return true;
    while ((pos = ip.find(".")) != std::string::npos) {
        segment = ip.substr(0, pos);
        num = std::atoi(segment.c_str());
        if (num < 0 || num > 255)
            return false;
        ip = ip.substr(pos + 1);
    }
    num = std::atoi(ip.c_str());
    if (num < 0 || num > 255)
        return false;
    return true;
}

bool    ConfigParser::validPort(const std::string& port) {
    int num = std::atoi(port.c_str());
    if (num < 0 || num > 65535)
        return false;
    return true;
}

bool    ConfigParser::validAutoindex(const std::string& line) {
    if (line.find("on") != std::string::npos || line.find("off") != std::string::npos)
        return true;
    return false;
}

bool    ConfigParser::validErrorPage(const std::string& line) {
    std::list<std::string> tmp_list = splitString<std::list<std::string> >(line, ' ');
    while (!tmp_list.empty()) {
        const std::string& token = tmp_list.front();
        if (onlyDigits(token) && std::atoi(token.c_str()) >= 300 && std::atoi(token.c_str()) <= 599) {
            tmp_list.pop_front();
            continue;
        }
        if (token[0] == '/' && tmp_list.size() == 1)
            return true;
        tmp_list.pop_front();
    }
    return false;
}

bool    ConfigParser::validReturn(const std::string& line) {
    std::list<std::string> tmp_list = splitString<std::list<std::string> >(line, ' ');
    if (tmp_list.size() > 2 || tmp_list.size() == 0)
        return false;
    if (tmp_list.size() == 1) {
        const std::string& value = tmp_list.front();
        if (onlyDigits(value) && std::atoi(value.c_str()) >= 300 && atoi(value.c_str()) <= 599)
            return true;
        return false;
    }
    if (tmp_list.size() == 2) {
        const std::string&  status_code = tmp_list.front();
        const std::string&  path = tmp_list.back();
        if (!onlyDigits(status_code) || std::atoi(status_code.c_str()) < 300 || std::atoi(status_code.c_str()) > 599)
            return false;
        if (path.find("https://") == std::string::npos && path.find("http://") == std::string::npos && !validRoot(path))
            return false;
        return true;
    }   
    return false;
}

bool    ConfigParser::validRoot(const std::string& line) {
    struct stat buffer;
    if (line.empty() || line[0] != '/')
        return false;
    // if (stat(line.c_str(), &buffer) == -1 || S_ISDIR(buffer.st_mode) == 0)
    //     return false;
    return true;
}

bool    ConfigParser::validBodySize(const std::string& line) {
    unsigned long num = std::stoul(line);
    if (num < 0 || std::isalpha(line[line.size() - 2]))
        return false;
    if (onlyDigits(line) || line[line.size() - 1] == 'k' || line[line.size() - 1] == 'K' ||
            line[line.size() - 1] == 'm' || line[line.size() - 1] == 'M')
        return true;
    return false;
}

bool    onlyDigits(const std::string& str) {
    for (size_t i = 0; i < str.size(); i++) {
        if (!std::isdigit(str[i]))
            return false;
    }
    return true;
}

void    removeWhitespaces(std::string& str) { 
    size_t  start = str.find_first_not_of(" \t");
    size_t  end = str.find_last_not_of(" \t");

    if (start == std::string::npos)
        str = "";
    else
        str = str.substr(start, end - start + 1);
}

bool    endingSemicolon(const std::string& str) {
    return str[str.size() - 1] == ';';
}