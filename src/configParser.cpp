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
    if (file.empty())
        this->setFilePath("default.conf");
    else
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

bool ConfigParser::parseFile() {
    ConfigServer server;
    std::string line;
    std::ifstream file(this->getFilePath());

    if (!file.is_open())
        throw "Unable to open the file.";

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') { continue; }
        if (line.find("server {") != std::string::npos) {
            this->parseDirectives(file, server);
            setServers(server);
        }
    }
    if (file.is_open())
        file.close();
    return true;
}


bool    ConfigParser::parseDirectives(std::ifstream &file, ConfigServer &server) {
    std::string line;
    while (getline(file, line)) {
        if (line.find("}") != std::string::npos) { return true; }
        if (line.empty() || line[0] == '#') { continue; }
        if (line.find("listen ") != std::string::npos && line.find(";") != std::string::npos) {
            if (line.find(":") != std::string::npos) {
                server.setIp(line.substr(line.find(" ") + 1, line.find(":")));
                server.setPort(line.substr(line.find(":") + 1, line.find(";") - line.find(":") - 1));
            }
            else { 
                if (line.find(".") != std::string::npos)
                    server.setIp(line.substr(line.find(" ") + 1, line.find(";")));
                else
                    server.setPort(line.substr(line.find(" ") + 1, line.find(";")));
            }
        }
        else if (line.find("server_name ") != std::string::npos) {
            std::string server_name = line.substr(line.find(" ") + 1, line.find(";"));
            server.setServerNames(server_name);
        }
        else if (line.find("root ") != std::string::npos)
            server.setRoot(line.substr(line.find(" ") + 1, line.find(";")));
        else if (line.find("limit_client_body_size ") != std::string::npos)
            server.setLimitClientBodySize(line.substr(line.find(" ") + 1, line.find(";")));
        else if (line.find("error_page ") != std::string::npos) {
            server.setErrorPages(line.substr(line.find(" ") + 1, line.find(";")));
        }
        else if (line.find("location {") != std::string::npos) {
            ConfigLocation location;
            parseLocation(file, line, location);
            server.setLocations(location);
        }
        return false;
    }
}

bool ConfigParser::parseLocation(std::ifstream &file, std::string line, ConfigLocation &location) {
    size_t start_pos = line.find(" ") + 1;
    size_t end_pos = line.find(";");
    std::string path = line.substr(start_pos, line.find("{") - start_pos);
    location.setPath(path);
    while (getline(file, line)) {
        if (line.find("}") != std::string::npos) { return true; }
        if (line.empty() || line[0] == '#') { continue; }
        if (line.find("root ") != std::string::npos)
            location.setRoot(line.substr(line.find(" ") + 1, end_pos - start_pos));
        else if (line.find("autoindex ") != std::string::npos)
            location.setAutoindex(line.substr(line.find(" ") + 1, line.find(";")));
        else if (line.find("index ") != std::string::npos)
            location.setIndex(line.substr(line.find(" ") + 1, line.find(";")));
        else if (line.find("allowed_methods ") != std::string::npos)
            location.setAllowedMethods(line.substr(line.find(" ") + 1, line.find(";")));
        else if (line.find("return ") != std::string::npos)
            location.setReturn(line.substr(line.find(" ") + 1, line.find(";")));
        
    }
    return false;
}

bool    ConfigParser::validMethods(const std::set<std::string>& methods) {
    std::set<std::string> validMethods = {"GET", "POST", "DELETE", "PUT"};
    for (std::set<std::string>::iterator it = methods.begin(); it != methods.end(); ++it) {
        if (validMethods.find(*it) == validMethods.end())
            return false;
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
        num = stoi(segment);
        if (num < 0 || num > 255)
            return false;
        ip = ip.substr(pos + 1);
    }
    num = stoi(ip);
    if (num < 0 || num > 255)
        return false;
    return true;
}

bool    ConfigParser::validPort(const std::string& port) {
    int num = std::stoi(port);
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
    std::list<std::string> tmp_list = splitString<std::list<std::string>>(line, ' ');
    while (!tmp_list.empty()) {
        const std::string& token = tmp_list.front();
        if (token[0] == '/') {
            if (tmp_list.size() > 1)
                return false;
            return true;
        }
        if (!onlyDigits(token)|| stoi(token) < 300 || stoi(token) > 599)
            return false;
        tmp_list.pop_front();
    }
    return false;
}

bool    ConfigParser::validReturn(const std::string& line) {
    std::list<std::string> tmp_list = splitString<std::list<std::string>>(line, ' ');
    if (tmp_list.size() > 2 || tmp_list.size() == 0)
        return false;
    if (tmp_list.size() == 1) {
        const std::string& value = tmp_list.front();
        if (value.find("https://") != std::string::npos || value.find("http://") != std::string::npos)
            return true;
        if (onlyDigits(value) && stoi(value) >= 300 && stoi(value) <= 599)
            return true;
        return false;
    }
    if (tmp_list.size() == 2) {
        const std::string&  status_code = tmp_list.front();
        const std::string&  path = tmp_list.back();
        if (!onlyDigits(status_code) || stoi(status_code) < 300 || stoi(status_code) > 599)
            return false;
        if (path.find("https://") == std::string::npos && path.find("http://") == std::string::npos)
            return false;
        return true;
    }   
    return false;
}

bool    ConfigParser::validRoot(const std::string& line) {
    struct stat buffer;
    if (line.empty() || line[0] != '/')
        return false;
    if (stat(line.c_str(), &buffer) == -1 || S_ISDIR(buffer.st_mode) == 0)
        return false;
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
    return str.find_first_not_of("0123456789") == std::string::npos;
}
