/*By parsing configuration file following the rule of nginx config file, 
save the data in a struct and return a list of servers info.*/

#include "../include/configParser.hpp"

/*Pseudo code
1. Read the configuration file line by line.
2. Parse the file.
3. Initailize a list of configServer.
4. Check format of the configuration file.
5. Check validity of the configuration file.
6. Save the data in a struct and return it.
*/

configParser::configParser(const std::string& file) {
    if (file.empty())
        this->setFilePath("default.conf");
    else
        this->setFilePath(file);
}

void    configParser::setFilePath(const std::string& file) {
    std::ifstream   test(file);
    if (!test.is_open())
        throw   "Configuration file cannot be opened.";
    this->filePath = file;
}

void    configParser::setServers(configServer server) {

}

bool configParser::parseFile() {
    configServer server;
    std::string line;
    std::ifstream file(this->getFilePath());

    if (!file.is_open())
        throw "Unable to open the file.";

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') { continue; }
        if (line.find("server {") != std::string::npos) {
            this->parseDirectives(file, server);
            servers.push_back(server);
        }
    }
    if (file.is_open())
        file.close();
    return true;
}

bool configParser::parseLocation(std::ifstream &file, std::string line, configLocation &location) {
    std::string path = line.substr(line.find(" ") + 1, line.find("{"));
    location.setPath(path);
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') { continue; }
        if (line.find("root ") != std::string::npos)
            location.setRoot(line.substr(line.find(" ") + 1, line.find(";")));
        else if (line.find("autoindex ") != std::string::npos) {
            if (line.find("on") != std::string::npos)
                location.setAutoindex(true);
            else if (line.find("off") != std::string::npos)
                location.setAutoindex(false);
            else
                throw "Invalid autoindex value.";
        }
        else if (line.find("index ") != std::string::npos) {

        }
        else if (line.find("allowed_methods ") != std::string::npos) {

        }
        else if (line.find("return ") != std::string::npos) {

        }
        if (line.find("}") != std::string::npos) {
            return true;
        }
    }
}

bool    configParser::validLocation(std::vector<configLocation> locations) {
    for (std::vector<configLocation>::iterator it = locations.begin(); it != locations.end(); ++it) {
        //To do
    }
    return true;
}

bool    configParser::validMethods(std::set<std::string> methods) {
    std::set<std::string> validMethods = {"GET", "POST", "DELETE", "PUT"};
    for (std::set<std::string>::iterator it = methods.begin(); it != methods.end(); ++it) {
        if (validMethods.find(*it) == validMethods.end())
            return false;
    }
    return true;
}
bool    configParser::validIp(std::string ip) {
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

//Using exception handling to check the validity of the configuration file.
void    configParser::checkValidity(const std::string& directive, configServer &server) {
    if (directive == "listen") {
        if (server.getPort() < 0 || server.getPort() > 65535)
            throw "Invalid port number.";
        if (server.getIp().find(".") == std::string::npos && server.getIp() != "localhost" && !this->validIp(server.getIp()))
            throw "Invalid IP address.";
    } else if (directive == "server_name") {
        if (server.getServerNames().empty())
            throw "No server name specified.";
    } else if (directive == "root") {
        if (server.getRoot().empty())
            throw "No root directory specified.";
    } else if (directive == "limit_client_body_size") {
        if (server.getLimitClientBodySize() < 0)
            throw "Invalid client body size limit.";
    } else if (directive == "error_page") {
        if (server.getDefaultErrorPages().empty())
            throw "No error page specified.";
    } else if (directive == "location") {
        if (server.getLocations().empty() || validLocation(server.getLocations()))
            throw "No location specified.";
    }
    //To add more directives
}

void    checkFormat(const std::string& line, const std::string& directive) {
    //To do
    if (directive == "listen") {
        if (line.find(":") == std::string::npos)
            throw "Invalid " + directive + " format.";
    } else if (directive == "server_name") {
        if (line.find(" ") == std::string::npos)
            throw "Invalid " + directive + " format.";
    } else if (directive == "root") {
        if (line.find(" ") == std::string::npos)
            throw "Invalid " + directive + " format.";
    } else if (directive == "limit_client_body_size") {
        if (line.find(" ") == std::string::npos)
            throw "Invalid " + directive + " format.";
    } else if (directive == "error_page") {
        if (line.find(" ") == std::string::npos)
            throw "Invalid " + directive + " format.";
    } else if (directive == "location") {
        if (line.find(" ") == std::string::npos)
            throw "Invalid " + directive + " format.";
    }
}

bool    configParser::parseDirectives(std::ifstream &file, configServer &server) {
    std::string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') { continue; }
        if (line.find("listen ") != std::string::npos && line.find(";") != std::string::npos) {
            if (line.find(":") != std::string::npos) {
                server.setIp(line.substr(line.find(" ") + 1, line.find(":")));
                server.setPort(stoi(line.substr(line.find(":") + 1, line.find(";"))));
            }
            else { 
                if (line.find(".") != std::string::npos)
                    server.setIp(line.substr(line.find(" ") + 1, line.find(";"))); //will save the ip address or "localhost"
                else
                    server.setPort(stoi(line.substr(line.find(" ") + 1, line.find(";"))));
            }
        }
        else if (line.find("server_name ") != std::string::npos) {
            std::string server_name = line.substr(line.find(" ") + 1, line.find(";"));
            while (server_name.find(" ") != std::string::npos) {
                server.setServerNames(server_name.substr(0, server_name.find(" ")));
                server_name = server_name.substr(server_name.find(" ") + 1);
            }
            server.setServerNames(server_name);
        }
        else if (line.find("root ") != std::string::npos)
            server.setRoot(line.substr(line.find(" ") + 1, line.find(";")));
        else if (line.find("limit_client_body_size ") != std::string::npos)
            server.setLimitClientBodySize(stoi(line.substr(line.find(" ") + 1, line.find(";"))));
        else if (line.find("error_page ") != std::string::npos) {
            errorPage error_page;
            std::string error_codes = line.substr(line.find(" ") + 1, line.find(";"));
            while (error_codes.find(" ") != std::string::npos) {
                error_page.error_codes.insert(stoi(error_codes.substr(0, error_codes.find(" "))));
                error_codes = error_codes.substr(error_codes.find(" ") + 1);
            }
            error_page.error_path = error_codes;
            server.setDefaultErrorPages(error_page);
        }
        else if (line.find("location") != std::string::npos) {
            configLocation location;
            parseLocation(file, line, location);
            server.setLocations(location);
        }
    }
}