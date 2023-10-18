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

unordered_map<string, string> Store::lineParse(string data) {
    unordered_map<string, string> dict;
    string temp = "";
    int size = data.size();
    pair<string, string> p;
    int i = 0;
    while (i < size) {
        if (data[i] == '%') {
            string hex;
            hex.push_back(data[i + 1]);
            hex.push_back(data[i + 2]);
            int dec = stoi(hex, nullptr, 16);
            temp.push_back(static_cast<char>(dec));
            i += 3;
        }
        else if (data[i] == '=') {
            p.first = temp;
            temp = "";
        }
        else if (data[i] == '&') {
            p.second = temp;
            temp = "";
            dict[p.first] = p.second;
        }
        else if (data[i] == '+') {
            temp.push_back(' ');
        }
        else {
            temp.push_back(data[i]);
        }
        i++;
    }

    p.second = temp;
    dict[p.first] = p.second;

    return dict;
}

string Store::lineUnparse(unordered_map<string, string> dict) {
    string res;
    for (auto i : dict) {
        res += i.first + "=" + i.second + "&";
    }
    res.pop_back();
    return res;
}

string Store::linePretty(string id, unordered_map<string, string> dict) {
    string res = "ID : " + id + "; ";
    for (auto i : dict) {
        res += i.first + " : " + i.second + "; ";
    }
    return res;
}

unordered_map<string, unordered_map<string, string>> Store::parse(ifstream& in) {
    unordered_map<string, unordered_map<string, string>> dict;

    string line;
    while (getline(in, line)) {
        auto temp = lineParse(line);
        string ID = temp["id"];
        temp.erase("id");
        dict[ID] = temp;
    }

    return dict;
}

Store::Store() {
    name = "temp.txt";
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
    auto dict = lineParse(line);
    for (auto i : dict) {
        cout << i.first << " : " << i.second << endl;
    }
    string unparse = lineUnparse(dict);
    cout << unparse;
}

string Store::AppendData(string record) {
    string id = getNextId();
    data[id] = lineParse(record);
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
        file << "id=" << i.first << "&" << lineUnparse(data[i.first]) << endl;
    }
    file.close();
}