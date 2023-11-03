#include "http.hpp"

HTTP::HTTP() {
    query       = Helper::getenvHandler("QUERY_STRING");
    method      = Helper::getenvHandler("REQUEST_METHOD");
    cookie      = Helper::getenvHandler("HTTP_COOKIE");
    contentType = Helper::getenvHandler("CONTENT_TYPE");

    if( cookie != "" ) {
        cookieParams = Helper::parseCookies(cookie);
    }
    
    if( query != "" ) {
        queryParams = Helper::parseForm(query);
    }

    if ( method == "POST" ) {
        std::cin >> body;
        if( body != "" ) {
            if ( std::string( contentType ).find( "application/x-www-form-urlencoded" ) != std::string::npos ) {
                bodyParams = Helper::parseForm( body );
            } else if ( std::string( contentType ).find( "multipart/form-data" ) != std::string::npos ) {
                //TODO
            } else {
                //TODO
            }
        }
    }
}

std::string HTTP::getMethod() {
    return method;
}

std::string HTTP::getBody() {
    return body;
}

std::map<std::string, std::string> HTTP::GetBodyParams() {
    return bodyParams;
}

std::string HTTP::toString() {
    std::string str;

    str += "<p>query: "         + query         + "</p>";
    str += "<p>method: "        + method        + "</p>";
    str += "<p>cookie: "        + cookie        + "</p>";
    str += "<p>contentType: "   + contentType   + "</p>";
    str += "<p>body: "          + body          + "</p>";


    str += "query params: ";
    for(auto i : queryParams) {
        str += i.first + " : " + i.second + "; ";
    }

    str += "</p>";

    str += "body params: ";
    for(auto i : bodyParams) {
        str += i.first + " : " + i.second + "; ";
    }

    str += "</p>";

    str += "cookie params: ";
    for(auto i : cookieParams) {
        str += i.first + " : " + i.second + "; ";
    }

    str += "</p>";
    return str;
}

std::string HTTP::httpGet(std::string name) {
    if(queryParams.find(name) != queryParams.end()) {
        return queryParams[name];
    }
    return "";
}

std::string HTTP::httpPost(std::string name) {
    if(bodyParams.find(name) != bodyParams.end()) {
        return bodyParams[name];
    }
    return "";
}

std::string HTTP::getCookie(std::string name) {
    if(cookieParams.find(name) != cookieParams.end()) {
        return cookieParams[name];
    }
    return "";
}

std::string HTTP::setCookie(std::string name, std::string value) {
    std::cout << "Set-Cookie: " + name + "=" + value + "; \n";
    return "";
}

HTTP::~HTTP() {

}