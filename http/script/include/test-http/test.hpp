#ifndef TEST_HPP
#define TEST_HPP

#include <iostream>
#include <string>

class TestHTTP {
    const std::string ENV[ 24 ] = {
        "COMSPEC", 
        "DOCUMENT_ROOT", 
        "GATEWAY_INTERFACE",
        "HTTP_ACCEPT",
        "HTTP_ACCEPT_ENCODINGS",
        "HTTP_ACCEPT_LANGUAGE",
        "HTTP_CONNECTION",
        "HTTP_HOST",
        "HTTP_USER_AGENT",
        "PATH",
        "QUERY_STRING",
        "REMOTE_ADDR",
        "REMOTE_PORT",
        "REQUEST_METHOD",
        "REQUEST_URI",
        "SCRIPT_FILENAME",
        "SCRIPT_NAME",
        "SERVER_ADDR",
        "SERVER_ADMIN",
        "SERVER_NAME",
        "SERVER_PORT",
        "SERVER_PROTOCOL",
        "SERVER+SIGNATURE",
        "SERVER_SOFTWARE"
    };
    public:
    int testMain();
};

#endif