#ifndef STORE_HPP
#define STORE_HPP

#include <map>
#include <string>
#include <fstream>
#include "../helper/helper.hpp"

class Store {
private:
    std::string name;
    std::map<std::string, std::map<std::string, std::string>> data;

    std::string getNextId();
    std::string linePretty(std::string id, std::map<std::string, std::string> dict);
    std::map<std::string, std::map<std::string, std::string>> parse(std::ifstream& in);

public:
    Store();
    Store(std::string fileName);
    std::string AppendData(std::string record);
    std::string AppendData(std::map<std::string, std::string> dict);
    std::string DeleteOne(std::string ID);
    std::string GetOne(std::string ID);
    std::string GetAll();
    bool emailValidation(std::string email);
    bool dataValidation(std::string email, std::string pass);
    ~Store();
};

#endif // STORE_HPP
