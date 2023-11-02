#include <iostream>

#include "include/http/http.hpp"

int main() {
    HTTP http;

    if (http.getMethod() == "POST") {

        std::cout << "Content-type: text/html\n";

        for (auto item : http.bodyParams) {
            http.setCookie(item.first, item.second);
        }

        std::cout << "\n\n";
        std::cout << "Done";
    }

    return 0;
}
