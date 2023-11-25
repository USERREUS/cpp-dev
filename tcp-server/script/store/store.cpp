#include "store.hpp"

std::string Store::getNextId() {
    int max_id = 0;
    for (auto i : data) {
        int id = std::stoi(i.first);
        if (id > max_id) {
            max_id = id;
        }
    }
    return std::to_string(max_id + 1);
}

std::string Store::linePretty(std::string id, std::map<std::string, std::string> dict) {
    std::string res = "ID : " + id + "; ";
    for (auto i : dict) {
        res += Helper::urlDecode(i.first) + " : " + Helper::urlDecode(i.second) + "; ";
    }
    return res;
}

std::map<std::string, std::map<std::string, std::string>> Store::parse(std::ifstream& in) {
    std::map<std::string, std::map<std::string, std::string>> dict;

    std::string line;
    while (std::getline(in, line)) {
        auto temp = Helper::parseForm(line);
        std::string ID = temp["id"];
        temp.erase("id");
        dict[ID] = temp;
    }

    return dict;
}

Store::Store() {
    name = "store.txt";
    std::ifstream file(name);
    data = parse(file);
    file.close();
}

Store::Store(std::string fileName) {
    name = fileName;
    std::ifstream file(name);
    data = parse(file);
    file.close();
}

std::string Store::AppendData(std::string record) {
    std::string id = getNextId();
    data[id] = Helper::parseForm(record);
    return "Success AppendData";
}

std::string Store::AppendData(std::map<std::string, std::string> dict) {
    std::string id = getNextId();
    std::string record = Helper::encodeData(dict); // without urlEncode
    data[id] = Helper::parseForm(record);
    return "Success AppendData";
}

std::string Store::DeleteOne(std::string ID) {
    if (data.find(ID) == data.end()) {
        return "Delete Error. ID not found.";
    }
    data.erase(ID);
    return "Success";
}

std::string Store::GetOne(std::string ID) {
    if (data.find(ID) == data.end()) {
        return "GetOne error : ID not found";
    }
    return linePretty(ID, data[ID]);
}

std::string Store::GetAll() {
    if (data.size() > 0) {
        std::string res;
        for (auto i : data) {
            res += linePretty(i.first, i.second) + "</p>";
        }
        return res;
    }
    return "Store is empty";
}

bool Store::emailValidation(std::string email) {
    for (auto rec : data) {
        if (rec.second.find("email") != rec.second.end()) {
            if (rec.second["email"] == email) {
                return false;
            }
        }
    }
    return true;
}

bool Store::dataValidation(std::string email, std::string pass) {
    for (auto rec : data) {
        if (rec.second.find("email") != rec.second.end() && rec.second.find("password") != rec.second.end()) {
            if (rec.second["email"] == email && rec.second["password"] == pass) {
                return true;
            }
        }
    }
    return false;
}

Store::~Store() {
    std::ofstream file(name);
    for (auto i : data) {
        file << "id=" << i.first << "&" << Helper::encodeData(data[i.first]) << std::endl;
    }
    file.close();
}
