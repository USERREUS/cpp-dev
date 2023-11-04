#include <iostream>

#include "include/http/http.hpp"
#include "include/store/store.hpp"

int main() {

    HTTP http;
    Store store;

    if (http.getMethod() == "POST") {

        store.AppendData(http.body);
        std::cout << "Content-type: text/html\n";

        for (auto item : http.bodyParams) {
            http.setCookie(item.first, item.second);
        }

        std::cout << "\n\n";
        std::cout << "<p>Done</p>\n";

    } else if (http.getMethod() == "GET") {

        std::cout << "Content-type: text/html\n\n";
        std::string action = http.httpGet("action");
        std::string id = http.httpGet("id");

        if(action == "findAll") {
            std::cout << store.GetAll() << std::endl;
        }
        else if(action == "findOne") {
            std::cout << store.GetOne(id) << std::endl;
        }
        else if(action == "deleteOne") {
            std::cout << store.DeleteOne(id) << std::endl;
        }
        else if(action == "print") {
            std::cout << http.toString() << std::endl;
        } 
        else {
            std::cout << "Unknown action" << std::endl;
        }

    } else {

        std::cout << "Content-type: text/html\n\n";
        std::cout << "<p>Unknown method</p>\n";

    }

    return 0;
}
