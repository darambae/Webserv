#pragma once

#include "webserv.hpp"

class ConfigServer;
class ConfigLocation;

class   ConfigParser {
    private:

        std::vector<ConfigServer>  _servers;
        std::string _filePath;

    public:

        ConfigParser(const std::string& filePath);
        ~ConfigParser() {};

        const   std::string&    getFilePath() const { return this->_filePath; }
        std::vector<ConfigServer>    getServers() const { return this->_servers; }

        void    setFilePath(const std::string& file);
        void    setServers(const ConfigServer& server);

        void    parseFile();
        void    parseLocation(std::ifstream &file, std::string line, ConfigLocation &location);
        void    parseDirectives(std::ifstream &file, ConfigServer &server);
        

        static void    validMethods(const std::string& methods);
        static void    validIp(std::string ip);
        static void    validPort(const std::string& port);
        static void    validAutoindex(const std::string& line);
        static void    validErrorPage(const std::string& line);
        static void    validReturn(const std::string& line);
        static void    validRoot(const std::string& line);
        static void    validBodySize(const std::string& line);
        static void    validCgiExtension(const std::string& line);

};
