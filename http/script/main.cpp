#include <iostream>
#include <cstdlib>
#include <cstring>
#include <map>

#include "include/http/http.hpp"


int main() {
    HTTP http;

    std::cout << "Content-type: text/html\n\n";
    std::cout << "<form action=\"handler.cgi\" method=\"POST\">\n";
    
    std::string name    = Helper::urlDecode(http.getCookie("name"));
    std::string age     = Helper::urlDecode(http.getCookie("age"));
    std::string email   = Helper::urlDecode(http.getCookie("email"));

    std::cout << "Name: <input type=\"text\" name=\"name\" value=\"" << name << "\"required><br>\n";
    std::cout << "Age: <input type=\"number\" name=\"age\" value=\"" << age << "\"required><br>\n";
    std::cout << "Email: <input type=\"email\" name=\"email\" value=\"" << email << "\"required><br>\n";

    std::cout << "<input type=\"submit\" value=\"Post\">\n";
    std::cout << "</form>\n";
    std::cout << "<br><br>\n";

    std::cout << "<form action=\"handler.cgi\" method=\"GET\">\n";
    std::cout << "Action: <input type=\"text\" name=\"action\" value=\"findAll\"required><br>\n";
    std::cout << "ID: <input type=\"number\" name=\"id\" value=\"0\"required><br>\n";

    std::cout << "<input type=\"submit\" value=\"Get\">\n";
    std::cout << "</form>\n";
    std::cout << "<br><br>\n";

    std::cout << http.toString();

    std::cout << "</body></html>\n";
    return 0;
}
