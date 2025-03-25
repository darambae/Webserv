#pragma once

#include "webserv.hpp"

template <typename T>
std::string to_string(T value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}


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

std::string fullPath(const std::string &directoryName);
void        removeWhitespaces(std::string &str); // Remove whitespaces from a string in the beginning and the end
bool        endingSemicolon(const std::string& line);
bool        onlyDigits(const std::string& str); //Check if a string contains only digits
bool        validBracket(const std::string& line, char a, char b); //Check if a string contains a valid bracket(a), not b
uint64_t    get_time(void); //Get the current time in milliseconds
std::string isFoundIn(std::string subject, std::vector<std::string> vector); //Check if a string is found in a vector of strings
std::string generateHTMLstr(const std::string& title, const std::string& message, const std::string& opt); //Generate an HTML page
int 		stringToInt(std::string str);
