#ifndef STORE_HPP
#define STORE_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>

class Store {
    std::string name;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> data;

    std::string getNextId();
    std::unordered_map<std::string, std::string> lineParse(std::string data);
    std::string lineUnparse(std::unordered_map<std::string, std::string> dict);
    std::string linePretty(std::string id, std::unordered_map<std::string, std::string> dict);
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> parse(std::ifstream& in);

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