#include "redis_client.hpp"

RedisClient::RedisClient(const std::string& host, int port) {
    context_ = redisConnect(host.c_str(), port);
    if (context_ == nullptr || context_->err) {
        if (context_) {
            std::cerr << "Ошибка подключения к Redis: " << context_->errstr << std::endl;
        } else {
            std::cerr << "Не удалось выделить память для redisContext" << std::endl;
        }
        exit(1);
    }
}

RedisClient::~RedisClient() {
    if (context_) {
        redisFree(context_);
    }
}

void RedisClient::Set(const std::string& key, const std::string& value) {
    redisReply* reply = (redisReply*)redisCommand(context_, "SET %s %s", key.c_str(), value.c_str());
    freeReplyObject(reply);
}

std::string RedisClient::Get(const std::string& key) {
    redisReply* reply = (redisReply*)redisCommand(context_, "GET %s", key.c_str());
    std::string value;
    if (reply->type == REDIS_REPLY_STRING) {
        value = reply->str;
    }
    freeReplyObject(reply);
    return value;
}

std::vector<std::string> RedisClient::GetAllKeys() {
    redisReply* reply = (redisReply*)redisCommand(context_, "KEYS *");
    std::vector<std::string> keys;
    if (reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; ++i) {
            keys.push_back(reply->element[i]->str);
        }
    }
    freeReplyObject(reply);
    return keys;
}

void RedisClient::Delete(const std::string& key) {
    redisReply* reply = (redisReply*)redisCommand(context_, "DEL %s", key.c_str());
    freeReplyObject(reply);
}