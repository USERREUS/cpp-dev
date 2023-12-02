#include "store.hpp"

std::map<std::string, std::map<std::string, std::string>> Store::parse(std::ifstream& in) {
    std::map<std::string, std::map<std::string, std::string>> dict;

    std::string line;
    while (std::getline(in, line)) {
        auto temp = Helper::parseForm(line);
        std::string login = temp["login"];
        temp.erase("login");
        dict[login] = temp;
    }

    return dict;
}

std::string Store::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);

    std::string hashedPassword = "";
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        char hex[3];
        sprintf(hex, "%02x", hash[i]);
        hashedPassword += hex;
    }

    return hashedPassword;
}

Store::Store() {
    in.open(fileName);
    if (!in) {
        Helper::log(ERROR, "Store open error");
    } else {
        data = parse(in);
        in.close();
    }
}

Store::Store(std::string fileName) {
    in.open(fileName);
    if (!in) {
        Helper::log(ERROR, "Store open error");
    } else {
        data = parse(in);
        in.close();
    }
}

bool Store::SignUP(std::map<std::string, std::string>& dict) {
        if (dict.find(LOGIN) != dict.end() && dict.find(PASSWORD) != dict.end()) {
        std::string login = dict[LOGIN];
        std::string pass = dict[PASSWORD];
        if (data.find(login) == data.end()) {
            dict.erase(LOGIN);
            dict[PASSWORD] = hashPassword(pass);
            data[login] = dict;
            return true;
        }
    }
    Helper::log(ERROR, "Store: SignUP Error.");
    return false;
}

bool Store::SignIN(std::map<std::string, std::string>& dict) {
    if (dict.find(LOGIN) != dict.end() && dict.find(PASSWORD) != dict.end()) {
        std::string login = dict[LOGIN];
        std::string pass = dict[PASSWORD];
        if (data.find(login) != data.end() && data[login][PASSWORD] == hashPassword(pass)) {
            return true;
        }
    }
    return false;
}

Store::~Store() {
    out.open(fileName);
    if (!out) {
        Helper::log(ERROR, "Error store close");
    } else {
        for (auto rec : data) {
            out << "login=" << rec.first << "&" << Helper::encodeData(data[rec.first]) << std::endl;
        }
        out.close();
    }
}
