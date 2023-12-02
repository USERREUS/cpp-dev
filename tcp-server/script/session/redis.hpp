#ifndef REDIS_CLIENT_HPP
#define REDIS_CLIENT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <hiredis/hiredis.h>
#include "../helper/helper.hpp"

class RedisClient {
private:
    redisContext* context_;

public:
    RedisClient(const std::string& host = "127.0.0.1", int port = 6379);
    ~RedisClient();
    void Set(const std::string& key, const std::string& value);
    std::string Get(const std::string& key);
    std::vector<std::string> GetAllKeys();
    void Delete(const std::string& key);
};

#endif // REDIS_CLIENT_HPP
