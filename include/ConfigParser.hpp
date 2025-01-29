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

        void    setFilePath(const std::string& file);
        void    setServers(const ConfigServer& server);
        const   std::string&    getFilePath() const { return this->_filePath; }
        std::vector<ConfigServer>    getServers() const { return this->_servers; }

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

template <typename Container> //Return a container of strings split by delimiter
Container splitString(const std::string& line, char delimiter) {
    Container result;
    std::string token;
    std::istringstream tokenStream(line);

    while (std::getline(tokenStream, token, delimiter))
        result.insert(result.end(), token);
    return result;
}

template <typename Container> //Print a container
void printContainer(const Container& container) {
    typename Container::const_iterator it = container.begin();
    for (it = container.begin(); it != container.end(); ++it)
        std::cout << *it << " ";
    std::cout << std::endl;
}

void    removeWhitespaces(std::string& str); //Remove whitespaces from a string in the beginning and the end
bool    endingSemicolon(const std::string& line);
bool    onlyDigits(const std::string& str); //Check if a string contains only digits
bool    validBracket(const std::string& line, char a, char b); //Check if a string contains a valid bracket(a), not b