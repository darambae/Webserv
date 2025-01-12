/*By parsing configuration file following the rule of nginx config file, 
save the data in a struct and return a list of servers info.*/

#pragma once
#include "../include/configParser.hpp"
#include <fstream>
#include <cstring>

using namespace std;
/*Pseudo code
1. Read the configuration file line by line.
2. Parse the file.
3. Initailize a list of configServer.
4. Check format of the configuration file.
5. Check validity of the configuration file.
6. Save the data in a struct and return it.
*/
void    initDirectives(configServer &server) {
    server.ip = "*";
    server.port = 80;
    server.root = "";
    server.limit_client_body_size = -1;
}

list<configServer> parseConfigFile(string configFilePath) {
    list<configServer> servers;
    configServer server;
    string line;
    ifstream file(configFilePath);

    if (!file.is_open()) {
        cerr << "Error: Unable to open the configuration file." << endl;
        exit(EXIT_FAILURE);
    }
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') { continue; }
        if (line.find("server {") != string::npos) {
            parseDirectives(file, line, server);
            servers.push_back(server);
        }
        else { continue; }
    }
    file.close();
    return servers;
}

configLocation parseLocation(ifstream &file, string &line) {
    
}

bool    validMethods(set<string> methods) {
    set<string> validMethods = {"GET", "POST", "DELETE", "PUT"};
    for (set<string>::iterator it = methods.begin(); it != methods.end(); ++it) {
        if (validMethods.find(*it) == validMethods.end())
            return false;
    }
    return true;
}

bool    validIp(string ip) {
    size_t  pos;
    string  segment;
    int     num;
    
    while ((pos = ip.find(".")) != string::npos) {
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

bool    validLocation(vector<configLocation> locations) {
    for (vector<configLocation>::iterator it = locations.begin(); it != locations.end(); ++it) {
        //To do
    }
    return true;
}

//Using exception handling to check the validity of the configuration file.
void checkValidity(string directive, configServer &server) {
    if (directive == "listen") {
        if (server.port < 0 || server.port > 65535)
            throw "Invalid port number.";
        if (server.ip.find(".") == string::npos && server.ip != "localhost" && !validIp(server.ip))
            throw "Invalid IP address.";
    } else if (directive == "server_name") {
        if (server.server_names.empty())
            throw "No server name specified.";
    } else if (directive == "root") {
        if (server.root.empty())
            throw "No root directory specified.";
    } else if (directive == "limit_client_body_size") {
        if (server.limit_client_body_size < 0)
            throw "Invalid client body size limit.";
    } else if (directive == "error_page") {
        if (server.default_error_pages.empty())
            throw "No error page specified.";
    } else if (directive == "location") {
        if (server.locations.empty() || validLocation(server.locations))
            throw "No location specified.";
    }
    //To add more directives
}

void    checkFormat(string line, string directive) {
    //To do
    if (directive == "listen") {
        if (line.find(":") == string::npos)
            throw "Invalid " + directive + " format.";
    } else if (directive == "server_name") {
        if (line.find(" ") == string::npos)
           throw "Invalid " + directive + " format.";
    } else if (directive == "root") {
        if (line.find(" ") == string::npos)
           throw "Invalid " + directive + " format.";
    } else if (directive == "limit_client_body_size") {
        if (line.find(" ") == string::npos)
           throw "Invalid " + directive + " format.";
    } else if (directive == "error_page") {
        if (line.find(" ") == string::npos)
           throw "Invalid " + directive + " format.";
    } else if (directive == "location") {
        if (line.find(" ") == string::npos)
           throw "Invalid " + directive + " format.";
    }
}

configServer parseDirectives(ifstream &file, string &line, configServer &server) {
    initDirectives(server);
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') { continue; }
        if (line.find("listen ") != string::npos && line.find(";") != string::npos) {
            if (line.find(":") != string::npos) {
                server.ip = line.substr(line.find(" ") + 1, line.find(":"));
                server.port = stoi(line.substr(line.find(":") + 1, line.find(";")));
            }
            else { 
                if (line.find(".") != string::npos)
                    server.ip = line.substr(line.find(" ") + 1, line.find(";")); //will save the ip address or "localhost"
                else
                    server.port = stoi(line.substr(line.find(" ") + 1, line.find(";")));
            }
        }
        else if (line.find("server_name ") != string::npos) {
            string server_name = line.substr(line.find(" ") + 1, line.find(";"));
            while (server_name.find(" ") != string::npos) {
                server.server_names.push_back(server_name.substr(0, server_name.find(" ")));
                server_name = server_name.substr(server_name.find(" ") + 1);
            }
            server.server_names.push_back(server_name);
        }
        else if (line.find("root ") != string::npos)
            server.root = line.substr(line.find(" ") + 1, line.find(";"));
        else if (line.find("limit_client_body_size ") != string::npos)
            server.limit_client_body_size = stoi(line.substr(line.find(" ") + 1, line.find(";")));
        else if (line.find("error_page ") != string::npos) {
            errorPage error_page;
            string error_codes = line.substr(line.find(" ") + 1, line.find(";"));
            while (error_codes.find(" ") != string::npos) {
                error_page.error_codes.insert(stoi(error_codes.substr(0, error_codes.find(" "))));
                error_codes = error_codes.substr(error_codes.find(" ") + 1);
            }
            error_page.error_path = error_codes;
            server.default_error_pages.push_back(error_page);
        }
        else if (line.find("location") != string::npos) {
            configLocation location = parseLocation(file, line);
            server.locations.push_back(location);

        }
        else if (line.find("}") != string::npos)
            return server;
        if (line.find("}") != string::npos)
            return server;
    }
}