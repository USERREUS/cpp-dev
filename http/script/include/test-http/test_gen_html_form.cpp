#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unordered_map>

int main() {
    std::cout << "Content-type: text/html\n\n";
    
    std::cout << "<html><head><title>Form GET</title></head><body>";
    std::cout << "<h1>Form GET</h1>";
    std::cout << "<form action=\"test_set_cookies.cgi\" method=\"get\">";
    
    char* cookie_data = std::getenv("HTTP_COOKIE");
    std::unordered_map<std::string, std::string> cookieMap;
    std::string name, age;

    if (cookie_data) {
        char* token = std::strtok(cookie_data, "; ");

        while (token != nullptr) {
            char* equals = std::strchr(token, '=');
            if (equals != nullptr) {
                *equals = '\0';
                std::string key = token;
                std::string value = equals + 1;
                cookieMap[key] = value;
            }
            token = std::strtok(nullptr, "; ");
        }

        if (cookieMap.find("name") != cookieMap.end()) {
            name = cookieMap["name"];
        }

        if (cookieMap.find("age") != cookieMap.end()) {
            age = cookieMap["age"];
        }

    }

    std::cout << "Name: <input type=\"text\" name=\"name\" value=\"" << name << "\"><br>";
    std::cout << "Age: <input type=\"text\" name=\"age\" value=\"" << age << "\"><br>";
    std::cout << "<input type=\"submit\" value=\"Send\">";
    std::cout << "</form>";
    std::cout << "</body></html>";
    return 0;
}
