#ifndef HTTP_HPP
#define HTTP_HPP

#include "../helper/helper.hpp"

class HTTP {
public:
    std::string query;
    std::string method;
    std::string cookie;
    std::string body;
    std::string contentType;

    std::map<std::string, std::string> queryParams;
    std::map<std::string, std::string> bodyParams;
    std::map<std::string, std::string> cookieParams;

	HTTP();
    std::string getMethod();
    std::string getBody();
    std::map<std::string, std::string> GetBodyParams();
    std::string toString();
	std::string httpGet(std::string name);
	std::string httpPost(std::string name);
	std::string getCookie(std::string name);
	std::string setCookie(std::string name, std::string value);
    ~HTTP();
};

#endif