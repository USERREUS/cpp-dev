#include <iostream>
#include <cstdlib>
#include <cstring>
<<<<<<< HEAD
#include <map>
=======
#include <unordered_map>
>>>>>>> 5fb75eee5bf7b656a9927332a5ba5a984378777d

#include "include/http/http.hpp"


int main() {
    HTTP http;

    std::cout << "Content-type: text/html\n\n";
    std::cout << "<form action=\"handler.cgi\" method=\"POST\">\n";
    
<<<<<<< HEAD
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
=======
    std::string name = http.getCookie("name");
    std::string age = http.getCookie("age");
    std::string email = http.getCookie("email");

    std::cout << "Name: <input type=\"text\" name=\"name\" value=\"" << name << "\"required><br>\n";
    std::cout << "Agr: <input type=\"number\" name=\"age\" value=\"" << age << "\"required><br>\n";
    std::cout << "Email: <input type=\"email\" name=\"email\" value=\"" << email << "\"required><br>\n";

    std::cout << "<input type=\"submit\" value=\"Send\">\n";
    std::cout << "</form>\n";
    std::cout << "<br><br>\n";

    for( auto item : http.cookieParams ) {
        std::cout << "<p>|" << item.first << "|: " << item.second << ";</p>\n";
    }

    std::cout << "<p>Name: " << http.getCookie("name") << " : " << http.cookieParams["name"] << "</p>\n";
    std::cout << "<p>Age: " << http.getCookie("age") << " : " << http.cookieParams["age"] << "</p>\n";
    std::cout << "<p>Email: " << http.getCookie("email") << " : " << http.cookieParams["email"] << "</p>\n";
>>>>>>> 5fb75eee5bf7b656a9927332a5ba5a984378777d

    std::cout << http.toString();

    std::cout << "</body></html>\n";
    return 0;
}
