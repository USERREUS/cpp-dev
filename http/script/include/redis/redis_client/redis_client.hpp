#ifndef REDIS_CLIENT_HPP
#define REDIS_CLIENT_HPP

#include <hiredis/hiredis.h>
#include <string>
#include <vector>
#include <iostream>

class RedisClient {
    redisContext* context_;

public:
    RedisClient(const std::string& host, int port);
    ~RedisClient();
    void Set(const std::string& key, const std::string& value);
    std::string Get(const std::string& key);
    std::vector<std::string> GetAllKeys();
    void Delete(const std::string& key);
};

#endif
