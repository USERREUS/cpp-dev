#ifndef HELPER_HPP
#define HELPER_HPP

#include <map>
#include <string>

enum LogLevel {
    INFO,
    DEBUG,
    ERROR
};

const std::string LOGIN = "login";

class Helper {
public:
    static void log(LogLevel level, const std::string& message);
    static std::string readFile(const std::string& filename);
    static std::string encodeData(const std::map<std::string, std::string>& data);
    static std::string urlEncode(const std::string& data);
    static std::string trim(const std::string& s);
    static std::map<std::string, std::string> parseForm(const std::string &input);
    static std::map<std::string, std::string> parseCookies(const std::string& cookies);
    static std::string generateRandomString(int length);
    static std::string urlDecode(const std::string& str);
    static std::string extractHttpMethod(const std::string& httpRequest);
    static std::string extractHeader(const std::string& httpRequest, const std::string& headerName);
    static std::string extractQueryString(const std::string& httpRequest);
    static std::string extractPath(const std::string& httpRequest);
    static std::string changeSpecSymb(const std::string& str);
};

#endif // HELPER_HPP
