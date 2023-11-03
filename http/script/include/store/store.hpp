#ifndef STORE_HPP
#define STORE_HPP

<<<<<<< HEAD
#include "../helper/helper.hpp"
=======
#include <iostream>
#include <string>
#include <unordered_map>
>>>>>>> 5fb75eee5bf7b656a9927332a5ba5a984378777d
#include <fstream>

class Store {
    std::string name;
<<<<<<< HEAD
    std::map<std::string, std::map<std::string, std::string>> data;

    std::string getNextId();
    std::string linePretty(std::string id, std::map<std::string, std::string> dict);
    std::map<std::string, std::map<std::string, std::string>> parse(std::ifstream& in);
=======
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> data;

    std::string getNextId();
    std::unordered_map<std::string, std::string> lineParse(std::string data);
    std::string lineUnparse(std::unordered_map<std::string, std::string> dict);
    std::string linePretty(std::string id, std::unordered_map<std::string, std::string> dict);
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> parse(std::ifstream& in);
>>>>>>> 5fb75eee5bf7b656a9927332a5ba5a984378777d

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