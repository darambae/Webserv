//By parsing configuration file, we save the data in a struct.
#pragma once

#include "configServer.hpp"
#include "configLocation.hpp"
#include <cstring>
#include <fstream>
#include <list>
#include <set>

class   configParser {
    private:
        std::list<configServer>  servers;

        std::string filePath;
    public:
        configParser(const std::string& filePath);
        ~configParser() {};

        void    setFilePath(const std::string& file);
        void    setServers(const configServer& server);
        const   std::string& getFilePath() const { return this->filePath; }
        std::list<configServer>    getServers() const { return this->servers; }

        bool    parseFile();
        bool    parseLocation(std::ifstream &file, std::string line, configLocation &location);
        bool    parseDirectives(std::ifstream &file, configServer &server);
        
        void    checkValidity(const std::string& directive, configServer &server);
        bool    validMethods(const std::set<std::string>& methods);
        bool    validLocation(const std::vector<configLocation>& locations);
        bool    validIp(const std::string& ip);


    
};