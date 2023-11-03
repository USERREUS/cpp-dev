#include "store.hpp"

using namespace std;

string Store::getNextId() {
    int max_id = 0;
    for (auto i : data) {
        int id = stoi(i.first);
        if (id > max_id) {
            max_id = id;
        }
    }
    return to_string(max_id + 1);
}

string Store::linePretty(string id, map<string, string> dict) {
    string res = "ID : " + id + "; ";
    for (auto i : dict) {
        res += Helper::urlDecode(i.first) + " : " + Helper::urlDecode(i.second) + "; ";
    }
    return res;
}

map<string, map<string, string>> Store::parse(ifstream& in) {
    map<string, map<string, string>> dict;

    string line;
    while (getline(in, line)) {
        auto temp = Helper::parseForm(line);
        string ID = temp["id"];
        temp.erase("id");
        dict[ID] = temp;
    }

    return dict;
}

Store::Store() {
    name = "store.txt";
    ifstream file(name);
    data = parse(file);
    file.close();
}

Store::Store(string fileName) {
    name = fileName;
    ifstream file(name);
    data = parse(file);
    file.close();
}

void Store::TestLineRarse() {
    string line = "id=3&FirstName=Sergey&LastName=Sysoev&Email=test%40gmail.com";
    auto dict = Helper::parseForm(line);
    for (auto i : dict) {
        cout << i.first << " : " << i.second << endl;
    }
    string unparse = Helper::encodeData(dict);
    cout << unparse;
}

string Store::AppendData(string record) {
    string id = getNextId();
    data[id] = Helper::parseForm(record);
    return "Success AppendData";
}

string Store::DeleteOne(string ID) {
    if (data.find(ID) == data.end()) {
        return "Delete Error. ID not found.";
    }
    data.erase(ID);
    return "Success";
}

string Store::GetOne(string ID) {
    if (data.find(ID) == data.end()) {
        return "GetOne error : ID not found";
    }
    return linePretty(ID, data[ID]);
}

string Store::GetAll() {
    if (data.size() > 0) {
        string res;
        for (auto i : data) {
            res += linePretty(i.first, i.second) + "</p>";
        }
        return res;
    }
    return "Store is empty";
}

Store::~Store() {
    ofstream file(name);
    for (auto i : data) {
        file << "id=" << i.first << "&" << Helper::encodeData(data[i.first]) << endl;
    }
    file.close();

}