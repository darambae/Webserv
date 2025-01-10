//By parsing configuration file, we save the data in a struct.

#include <iostream>
#include <list>
#include <set>
#include <vector>

struct errorPage {
    std::set<int> error_codes;
    std::string error_path;
};
// struct to store the configuration of one location in a server
struct configLocation {
    std::string path;
    std::string root;
    bool autoindex;
    std::set<std::string> allowed_methods; 
    std::list<std::string> index;
    std::vector<std::string> cgi_extension;
    std::string cgi_path;
    std::list<errorPage> error_pages;
    int limit_client_body_size;
};
// struct to store the configuration of one server
struct configServer {
    std::string ip;
    int port;
    std::vector<std::string> server_names;
    std::string root;
    int limit_client_body_size;
    std::list<errorPage> default_error_pages;
    std::vector<configLocation> locations;
};

