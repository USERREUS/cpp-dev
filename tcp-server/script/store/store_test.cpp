#include "store.hpp"
#include "iostream"

int main() {
    Store store;
    std::map<std::string, std::string> testData = {
        { "login", "test" },
        {"password", "12345678"}
    };
    std::cout << store.SignIN(testData) << std::endl;

    testData[PASSWORD] = "123124124";
    std::cout << store.SignIN(testData) << std::endl;

    testData[LOGIN] = "user";
    std::cout << store.SignIN(testData) << std::endl;

    return 0;
}