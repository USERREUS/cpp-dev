#ifndef STORE_HPP
#define STORE_HPP

#include "../helper/helper.hpp"
#include <fstream>

class Store {
    std::string name;
    std::map<std::string, std::map<std::string, std::string>> data;

    std::string getNextId();
    std::string linePretty(std::string id, std::map<std::string, std::string> dict);
    std::map<std::string, std::map<std::string, std::string>> parse(std::ifstream& in);

public:
    Store();
    Store(std::string fileName);
    void TestLineRarse();
    std::string AppendData(std::string record);
    std::string DeleteOne(std::string ID);
    std::string GetOne(std::string ID);
    std::string GetAll();
    ~Store();
};

#endif