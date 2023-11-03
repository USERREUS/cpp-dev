#include <iostream>

#include "include/http/http.hpp"
<<<<<<< HEAD
#include "include/store/store.hpp"

int main() {

    HTTP http;
    Store store;

    if (http.getMethod() == "POST") {

        store.AppendData(http.body);
=======

int main() {
    HTTP http;

    if (http.getMethod() == "POST") {

>>>>>>> 5fb75eee5bf7b656a9927332a5ba5a984378777d
        std::cout << "Content-type: text/html\n";

        for (auto item : http.bodyParams) {
            http.setCookie(item.first, item.second);
        }

        std::cout << "\n\n";
<<<<<<< HEAD
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

=======
        std::cout << "Done";
>>>>>>> 5fb75eee5bf7b656a9927332a5ba5a984378777d
    }

    return 0;
}
