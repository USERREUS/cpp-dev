#ifndef STORE_HPP
#define STORE_HPP

#include <map>
#include <string>
#include <fstream>
#include "../helper/helper.hpp"
#include <openssl/sha.h>

//g++ -o test store_test.cpp ../helper/helper.cpp store.cpp -lcrypto

const std::string PASSWORD = "password"; //???

class Store {
private:
    std::string fileName = "store.txt";
    std::ifstream in;
    std::ofstream out;
    std::map<std::string, std::map<std::string, std::string>> data;
    std::map<std::string, std::map<std::string, std::string>> parse(std::ifstream& in);
    std::string hashPassword(const std::string& password);

public:
    Store();
    Store(std::string fileName);
    bool SignUP(std::map<std::string, std::string>& dict);
    bool SignIN(std::map<std::string, std::string>& dict);
    ~Store();
};

#endif // STORE_HPP
