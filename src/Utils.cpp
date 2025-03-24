#include "../include/Utils.hpp"

std::string   fullPath(const std::string& name) {
    struct stat buffer;
    char resolved_path[1000]; //possible memory leak
    //LOG_INFO("Given path name: " + name);
    std::string path = name[0] == '/' ? name.substr(1) : name;
    if (realpath(path.c_str(), resolved_path) == NULL)
        return "";
    //LOG_INFO("Resolved path : " + std::string(resolved_path));
    if (stat(resolved_path, &buffer) == -1)
        return "";
    return resolved_path;
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

//Checking if a string contains a single valid bracket(a) and not contains another bracket(b) in the string
bool    validBracket(const std::string& str, char a, char b) {
    return str.find(a) != std::string::npos && str.find(b) == std::string::npos &&
            str.find_first_of(a) == str.find_last_of(a);
}

uint64_t	get_time(void){
	struct timeval	time;
	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

std::string isFoundIn(std::string subject, std::vector<std::string> vector) {
	std::vector<std::string>::const_iterator it = vector.begin();
	for (; it != vector.end(); ++it) {
        LOG_DEBUG("Checking if " + subject + " is in " + *it);
        std::string tmp = *it;
		if (tmp.find(subject) != std::string::npos) {
			return *it;
		}
	}
	return "";
}

std::string generateHTMLstr(const std::string& title, const std::string& message, const std::string& opt) {
    std::ostringstream responseBody;
	responseBody << "<!DOCTYPE html>\n";
	responseBody << "<html lang=\"en\">\n";
	responseBody << "<head>\n";
	responseBody << "<meta charset=\"UTF-8\">\n";
	responseBody << "<link rel=\"stylesheet\" type=\"text/css\" href=\"/css/style.css\">\n";
	responseBody << "<title>" << title << "</title>\n";
	responseBody << "</head>\n";
	responseBody << "<body>\n";
	responseBody << "<h1>"<< message <<"</h1>\n";
	responseBody << opt;
	responseBody << "<p><a href=\"/\" class=\"button\">Go to Index Page</a></p>\n";
	responseBody << "</body>\n";
	responseBody << "</html>\n";
    return responseBody.str();
}

int stringToInt(std::string str) {
    std::stringstream ss(str);
    int result = -1;
    ss >> result;
    if (ss.fail())
        return -1;  // Valeur par défaut en cas d'échec
    return result;
}
