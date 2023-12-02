#ifndef SESSION_HPP
#define SESSION_HPP

#include <string>
#include <random>
#include <map>
#include "redis.hpp"

class Session {
public:
    Session();
    Session(std::string UUID);
    std::string getUUID();
    void setUUID(std::string UUID);
    void set(std::string name, std::string value);
    std::string get(std::string name);
    bool IsValid();
    ~Session();

private:
    const std::string CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghij";

    std::string UUID;
    RedisClient client;
    std::map<std::string, std::string> data;

    std::string generateUUID();
};

#endif // SESSION_HPP
