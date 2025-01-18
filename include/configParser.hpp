//By parsing configuration file, we save the data in a struct.
#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <set>
#include "ConfigServer.hpp"
#include "ConfigLocation.hpp"

class ConfigServer;
class ConfigLocation;

class   ConfigParser {
    private:
        std::list<ConfigServer>  servers;
        std::string filePath;

    public:
        ConfigParser(const std::string& filePath);
        ~ConfigParser() {};

        void    setFilePath(const std::string& file);
        void    setServers(const ConfigServer& server);
        const   std::string&    getFilePath() const { return this->filePath; }
        std::list<ConfigServer>    getServers() const { return this->servers; }

        void    parseFile();
        void    parseLocation(std::ifstream &file, std::string line, ConfigLocation &location);
        void    parseDirectives(std::ifstream &file, ConfigServer &server);
        
 
        static bool    validMethods(const std::set<std::string>& methods);
        static bool    validIp(std::string ip);
        static bool    validPort(const std::string& port);
        static bool    validAutoindex(const std::string& line);
        static bool    validErrorPage(const std::string& line);
        static bool    validReturn(const std::string& line);
        static bool    validRoot(const std::string& line);
        static bool    validBodySize(const std::string& line);


};

template <typename Container> //Return a container of strings split by delimiter
Container splitString(const std::string& line, char delimiter) {
    Container result;
    std::string token;
    std::istringstream tokenStream(line);

    while (std::getline(tokenStream, token, delimiter))
        result.insert(result.end(), token);
    return result;
}

bool    onlyDigits(const std::string& str); //Check if a string contains only digits