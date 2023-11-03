#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>

using namespace std;

class Store {
private:
    string name;

    unordered_map<string, unordered_map<string, string>> data;

    string getNextId() {
        int max_id = 0;
        for (auto i : data) {
            int id = stoi(i.first);
            if (id > max_id) {
                max_id = id;
            }
        }
        return to_string(max_id + 1);
    }

    unordered_map<string, string> lineParse(string data) {
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

    string lineUnparse(unordered_map<string, string> dict) {
        string res;
        for (auto i : dict) {
            res += i.first + "=" + i.second + "&";
        }
        res.pop_back();
        return res;
    }

    string linePretty(string id, unordered_map<string, string> dict) {
        string res = "ID : " + id + "; ";
        for (auto i : dict) {
            res += i.first + " : " + i.second + "; ";
        }
        return res;
    }

    unordered_map<string, unordered_map<string, string>> parse(ifstream& in) {
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

public:
    Store() {
        name = "temp.txt";
        ifstream file(name);
        data = parse(file);
        file.close();
    }

    Store(string fileName) {
        name = fileName;
        ifstream file(name);
        data = parse(file);
        file.close();
    }

    void TestLineRarse() {
        string line = "id=3&FirstName=Sergey&LastName=Sysoev&Email=test%40gmail.com";
        auto dict = lineParse(line);
        for (auto i : dict) {
            cout << i.first << " : " << i.second << endl;
        }
        string unparse = lineUnparse(dict);
        cout << unparse;
    }

    string AppendData(string record) {
        string id = getNextId();
        data[id] = lineParse(record);
        return "Success AppendData";
    }

    string DeleteOne(string ID) {
        if (data.find(ID) == data.end()) {
            return "Delete Error. ID not found.";
        }
        data.erase(ID);
        return "Success";
    }

    string GetOne(string ID) {
        if (data.find(ID) == data.end()) {
            return "GetOne error : ID not found";
        }
        return linePretty(ID, data[ID]);
    }

    string GetAll() {
        if (data.size() > 0) {
            string res;
            for (auto i : data) {
                res += linePretty(i.first, i.second) + "</p>";
            }
            return res;
        }
        return "Store is empty";
    }

    ~Store() {
        ofstream file(name);
        for (auto i : data) {
            file << "id=" << i.first << "&" << lineUnparse(data[i.first]) << endl;
        }
        file.close();
    }
};


class HTTP {

public:
    HTTP() {
        store = Store("store.txt");
        setContentType();
        //setCookie("name1", "val1");
        //cout << endl;

        //outputHTML("HTTP class created");
        //outputHTML(GetCookie("name1"));
        //outputHTML(getenv("HTTP_COOKIE"));
        //outputInfo();
        methodSelect();
    }

    //Принимает запись, сохраняет с новым id
    string httpPOST(string data) {
        return store.AppendData(data);
    }

    string httpGet(string data) {
        auto parsedQuery = dataParse(data);
        auto action = parsedQuery["action"];
        string ID = parsedQuery["id"];

        if (action == "findall") {
            return store.GetAll();
        }
        else if (action == "findone") {
            return store.GetOne(ID);
        }
        else if (action == "deleteone") {
            return store.DeleteOne(ID);
        }
        else if (action == "showcookies") {
            string cookieString = getenv("HTTP_COOKIE");
            auto dict = parseCookie(cookieString);
            string res;
            for (auto i : dict) {
                res += i.first + " : " + i.second + ";</p>";
            }
            return res;
        }
        else {
            return "Unknown action";
        }

        return "Internal server Error";
    }

    string GetCookie(string name) {
        string cookieString = getenv("HTTP_COOKIE");
        auto dict = parseCookie(cookieString);
        if (dict.find(name) == dict.end()) {
            return "Error: cookie \"" + name + "\" not found";
        }
        return "Cookie found: \"" + name + "\" = " + dict[name];
    }
    
    string setCookie(string name, string value) {
        cout << "Set-Cookie: " + name + "=" + value << endl;
        return "";
    }

    ~HTTP() {
    };

private:
    Store store;

    void setContentType() {
        cout << "Content-Type: text/html; charset=utf-8" << endl;
    }

    void outputDict(unordered_map<string, string> dict) {
        for (auto p : dict) {
            outputHTML(p.first + " : " + p.second);
        }
    }

    unordered_map<string, string> parseCookie(string cookie_string) {
        int size = cookie_string.size();
        pair<string, string> cookie;
        string temp = "";
        unordered_map<string, string> cookie_map;
        int i = 0;
        while (i < size) {
            if (cookie_string[i] == '=') {
                cookie.first = temp;
                temp = "";
            }
            else if (cookie_string[i] == ';') {
                cookie.second = temp;
                cookie_map[cookie.first] = cookie.second;
                temp = "";
                i++;
            }
            else {
                temp.push_back(cookie_string[i]);
            }
            i++;
        }
        cookie.second = temp;
        cookie_map[cookie.first] = cookie.second;
        return cookie_map;
    }

    unordered_map<string, string> dataParse(string data) {
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

    string setCookies(string data) {
        auto dict = dataParse(data);
        for (auto i : dict) {
            setCookie(i.first, i.second);
        }
        return "Success";
    }

    void methodSelect() {
        string method = getenv("REQUEST_METHOD");
        if (method == "POST") {
            string postData;
            cin >> postData;
            setCookies(postData);
            cout << endl;
            outputHTML(httpPOST(postData));
        }
        else if (method == "GET") {
            string getData = getenv("QUERY_STRING");
            cout << endl;
            outputHTML(httpGet(getData));
        }
        else {
            outputHTML("Unknown method");
        }
    }

    void outputHTML(string s) {
        cout << "<p>" << s << "</p>" << endl;
    }

    void outputInfo() {
        cout << "<p>1. DOCUMENT_ROOT: " << getenv("DOCUMENT_ROOT") << "</p>" << endl;
        cout << "<p>2. HTTP_REFERER: " << getenv("HTTP_REFERER") << "</p>" << endl;
        cout << "<p>3. HTTP_USER_AGENT: " << getenv("HTTP_USER_AGENT") << "</p>" << endl;
        cout << "<p>4. QUERY_STRING: " << getenv("QUERY_STRING") << "</p>" << endl;
        cout << "<p>5. REMOTE_ADDR: " << getenv("REMOTE_ADDR") << "</p>" << endl;
        cout << "<p>6. REQUEST_METHOD: " << getenv("REQUEST_METHOD") << "</p>" << endl;
        cout << "<p>7. SCRIPT_NAME: " << getenv("SCRIPT_NAME") << "</p>" << endl;
        cout << "<p>8. SERVER_NAME: " << getenv("SERVER_NAME") << "</p>" << endl;
        cout << "<p>9. SERVER_SOFTWARE: " << getenv("SERVER_SOFTWARE") << "</p>" << endl;
        cout << "<p>10. HTTP_COOKIE: " << getenv("HTTP_COOKIE") << "</p>" << endl;
    }
};

int main() {
    HTTP http = HTTP();
    return 0;
}