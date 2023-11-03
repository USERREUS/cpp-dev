#ifndef HTTP_HPP
#define HTTP_HPP

<<<<<<< HEAD
#include "../helper/helper.hpp"

class HTTP {
public:
=======
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <cctype>
#include <cstdlib>

class HTTP {
>>>>>>> 5fb75eee5bf7b656a9927332a5ba5a984378777d
    std::string query;
    std::string method;
    std::string cookie;
    std::string body;
    std::string contentType;

<<<<<<< HEAD
    std::map<std::string, std::string> queryParams;
    std::map<std::string, std::string> bodyParams;
    std::map<std::string, std::string> cookieParams;
=======
    std::string getenvHandler(const char* name);
    std::string urlDecode(const std::string& str);
    std::string trim(const std::string& s);
    std::unordered_map<std::string, std::string> parseForm(const std::string &data);
    std::unordered_map<std::string, std::string> parseCookies(const std::string& cookies);

public:
    std::unordered_map<std::string, std::string> queryParams;
    std::unordered_map<std::string, std::string> bodyParams;
    std::unordered_map<std::string, std::string> cookieParams;
>>>>>>> 5fb75eee5bf7b656a9927332a5ba5a984378777d

	HTTP();
    std::string getMethod();
    std::string getBody();
<<<<<<< HEAD
    std::map<std::string, std::string> GetBodyParams();
=======
    std::unordered_map<std::string, std::string> GetBodyParams();
>>>>>>> 5fb75eee5bf7b656a9927332a5ba5a984378777d
    std::string toString();
	std::string httpGet(std::string name);
	std::string httpPost(std::string name);
	std::string getCookie(std::string name);
	std::string setCookie(std::string name, std::string value);
    ~HTTP();
};

#endif