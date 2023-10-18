#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>

class Store {
private:
    std::string name;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> data;

    std::string getNextId() {
        int max_id = 0;
        for (auto i : data) {
            int id = stoi(i.first);
            if (id > max_id) {
                max_id = id;
            }
        }
        return std::to_string(max_id + 1);
    }

    std::unordered_map<std::string, std::string> lineParse(std::string data) {
        std::unordered_map<std::string, std::string> dict;
        std::string temp;
        int size = data.size();
        std::pair<std::string, std::string> p;
        int i = 0;
        while (i < size) {
            if (data[i] == '%') {
                std::string hex;
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

    std::string lineUnparse(std::unordered_map<std::string, std::string> dict) {
        std::string res;
        for (auto i : dict) {
            res += i.first + "=" + i.second + "&";
        }
        res.pop_back();
        return res;
    }

    std::string linePretty(std::string id, std::unordered_map<std::string, std::string> dict) {
        std::string res = "ID : " + id + "; ";
        for (auto i : dict) {
            res += i.first + " : " + i.second + "; ";
        }
        return res;
    }

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> parse(std::ifstream& in) {
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> dict;

        std::string line;
        while (std::getline(in, line)) {
            auto temp = lineParse(line);
            std::string ID = temp["id"];
            temp.erase("id");
            dict[ID] = temp;
        }

        return dict;
    }

public:
    Store() {
        name = "temp.txt";
        std::ifstream file(name);
        data = parse(file);
        file.close();
    }

    Store(std::string fileName) {
        name = fileName;
        std::ifstream file(name);
        data = parse(file);
        file.close();
    }

    std::string AppendData(std::string record) {
        std::string id = getNextId();
        data[id] = lineParse(record);
        return "Success AppendData";
    }

    std::string DeleteOne(std::string ID) {
        if (data.find(ID) == data.end()) {
            return "Delete Error. ID not found.";
        }
        data.erase(ID);
        return "Success";
    }

    std::string GetOne(std::string ID) {
        if (data.find(ID) == data.end()) {
            return "GetOne error : ID not found";
        }
        return linePretty(ID, data[ID]);
    }

    std::string GetAll() {
        if (data.size() > 0) {
            std::string res;
            for (auto i : data) {
                res += linePretty(i.first, i.second) + "</p>";
            }
            return res;
        }
        return "Store is empty";
    }

    ~Store() {
        std::ofstream file(name);
        for (auto i : data) {
            file << "id=" << i.first << "&" << lineUnparse(data[i.first]) << std::endl;
        }
        file.close();
    }
};

class Helper {

public:
    std::unordered_map<std::string, std::string> x_www_form_urlencoded_parse(const std::string &data) {
        std::unordered_map<std::string, std::string> dict;
        std::string temp;
        int size = data.size();
        std::pair<std::string, std::string> p;
        int i = 0;
        while (i < size) {
            if (data[i] == '%') {
                std::string hex;
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
    std::unordered_map<std::string, std::string> cookie_parse(std::string cookie_string) {
        int size = cookie_string.size();
        std::pair<std::string, std::string> cookie;
        std::string temp = "";
        std::unordered_map<std::string, std::string> cookie_map;
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
};

class HTTP {
    Helper helper;
    std::unordered_map<std::string, std::string> queryParams;
    std::unordered_map<std::string, std::string> bodyData;
    std::unordered_map<std::string, std::string> cookieData;

    std::string query;
    std::string method;
    std::string cookie;
    std::string body;

    std::string getenv_handler(const char* name) {
        if(getenv(name) != nullptr) {
            return getenv(name);
        }
        return "";
    }

public:
	HTTP() {
        query  = getenv_handler("QUERY_STRING");
        method = getenv_handler("REQUEST_METHOD");
        cookie = getenv_handler("HTTP_COOKIE");

        if(query != "") {
            queryParams = helper.x_www_form_urlencoded_parse(query);
        }

        if (method == "POST") {
            std::cin >> body;
            if(body != "") {
                bodyData = helper.x_www_form_urlencoded_parse(body);
            }
        }

        if(cookie != "") {
           cookieData = helper.cookie_parse(cookie);
        }
    }

    std::string GetMethod() {
        return method;
    }

    std::string GetBody() {
        return body;
    }

    std::unordered_map<std::string, std::string> GetBodyParsed() {
        return bodyData;
    }

    std::string toString() {
        std::string str;
        str += "query params: ";
        for(auto i : queryParams) {
            str += i.first + " : " + i.second + "; ";
        }

        str += "</p>";

        str += "body params: ";
        for(auto i : bodyData) {
            str += i.first + " : " + i.second + "; ";
        }

        str += "</p>";

        str += "cookies: ";
        for(auto i : cookieData) {
            str += i.first + " : " + i.second + "; ";
        }

        str += "</p>";
        return str;
    }

	std::string httpGet(std::string name) {
        if(queryParams.find(name) != queryParams.end()) {
            return queryParams[name];
        }
        return "";
    }

	std::string httpPost(std::string name) {
        if(bodyData.find(name) != bodyData.end()) {
            return bodyData[name];
        }
        return "";
    }

	std::string getCookie(std::string name) {
        if(cookieData.find(name) != cookieData.end()) {
            return cookieData[name];
        }
        return "";
    }

	std::string setCookie(std::string name, std::string value) {
        std::cout << "Set-Cookie: " + name + "=" + value << std::endl;
        return "";
    }
	
    ~HTTP() {

    }
};

void Test() {
    std::cout << "<p>1. DOCUMENT_ROOT: " << getenv("DOCUMENT_ROOT") << "</p>"       << std::endl;
    std::cout << "<p>2. HTTP_REFERER: " << getenv("HTTP_REFERER") << "</p>"         << std::endl;
    std::cout << "<p>3. HTTP_USER_AGENT: " << getenv("HTTP_USER_AGENT") << "</p>"   << std::endl;
    std::cout << "<p>4. QUERY_STRING: " << getenv("QUERY_STRING") << "</p>"         << std::endl;
    std::cout << "<p>5. REMOTE_ADDR: " << getenv("REMOTE_ADDR") << "</p>"           << std::endl;
    std::cout << "<p>6. REQUEST_METHOD: " << getenv("REQUEST_METHOD") << "</p>"     << std::endl;
    std::cout << "<p>7. SCRIPT_NAME: " << getenv("SCRIPT_NAME") << "</p>"           << std::endl;
    std::cout << "<p>8. SERVER_NAME: " << getenv("SERVER_NAME") << "</p>"           << std::endl;
    std::cout << "<p>9. SERVER_SOFTWARE: " << getenv("SERVER_SOFTWARE") << "</p>"   << std::endl;
    std::cout << "<p>10. HTTP_COOKIE: " << getenv("HTTP_COOKIE") << "</p>"          << std::endl;
}

int main() {
    HTTP http;
    Store store;
    std::string data;

    if(http.GetMethod() == "GET") {
        std::string action = http.httpGet("action");
        std::string id = http.httpGet("id");

        if(action == "findall") {
            data = store.GetAll();
        }
        else if(action == "findone") {
            data = store.GetOne(id);
        }
        else if(action == "print") {
            data = http.toString();
        }
    }
    else if(http.GetMethod() == "POST") {
        store.AppendData(http.GetBody());
        for(auto item : http.GetBodyParsed()) {
            http.setCookie(item.first, item.second);
        }
    } else {
        //TODO
    }

    std::cout << "Content-Type: text/html; charset=utf-8" << std::endl << std::endl;
    std::cout << data << std::endl;
    return 0;
}