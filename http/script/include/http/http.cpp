#include "http.hpp"

<<<<<<< HEAD
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
=======
std::string HTTP::getenvHandler(const char* name) { 
    if ( getenv(name) != nullptr )
        return getenv(name);
    
    return ""; 
}

std::string HTTP::urlDecode(const std::string& str) {
    std::string result;
    char hex[3] = {0};

    for (std::size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%' && i + 2 < str.length()) {
            hex[0] = str[i + 1];
            hex[1] = str[i + 2];
            result += static_cast<char>(std::stoi(hex, nullptr, 16));
            i += 2;
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }

    return result;
}

std::string HTTP::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    size_t end = s.find_last_not_of(" \t\n\r\f\v");

    if (start != std::string::npos && end != std::string::npos) {
        return s.substr(start, end - start + 1);
    } else {
        return "";
    }
}

std::unordered_map<std::string, std::string> HTTP::parseForm(const std::string &input) {
    std::unordered_map<std::string, std::string> result;
    std::size_t pos = 0;

    while (pos < input.length()) {
        std::size_t equalsPos = input.find('=', pos);
        if (equalsPos == std::string::npos) {
            break; // No '=' found, exit the loop
        }

        std::string key = input.substr(pos, equalsPos - pos);
        pos = equalsPos + 1;

        std::size_t ampersandPos = input.find('&', pos);
        if (ampersandPos == std::string::npos) {
            ampersandPos = input.length();
        }

        std::string value = input.substr(pos, ampersandPos - pos);
        pos = ampersandPos + 1;

        // URL-decode key and value
        key = trim(urlDecode(key));
        value = trim(urlDecode(value));

        result[key] = value;
    }

    return result;
}

std::unordered_map<std::string, std::string> HTTP::parseCookies(const std::string& cookies) {
    std::unordered_map<std::string, std::string> cookieMap;
    std::istringstream cookieStream(cookies);

    std::string cookie;
    while (std::getline(cookieStream, cookie, ';')) {
        std::istringstream cookiePairStream(cookie);
        std::string key, value;

        if (std::getline(cookiePairStream, key, '=')) {
            std::getline(cookiePairStream, value);
            key = trim(urlDecode(key));
            value = trim(urlDecode(value));
            cookieMap[key] = value;
        }
    }

    return cookieMap;
}

HTTP::HTTP() {
    query       = getenvHandler("QUERY_STRING");
    method      = getenvHandler("REQUEST_METHOD");
    cookie      = getenvHandler("HTTP_COOKIE");
    contentType = getenvHandler("CONTENT_TYPE");

    if( cookie != "" ) {
        cookieParams = parseCookies(cookie);
    }
    
    if( query != "" ) {
        queryParams = parseForm(query);
>>>>>>> 5fb75eee5bf7b656a9927332a5ba5a984378777d
    }

    if ( method == "POST" ) {
        std::cin >> body;
        if( body != "" ) {
            if ( std::string( contentType ).find( "application/x-www-form-urlencoded" ) != std::string::npos ) {
<<<<<<< HEAD
                bodyParams = Helper::parseForm( body );
=======
                bodyParams = parseForm( body );
>>>>>>> 5fb75eee5bf7b656a9927332a5ba5a984378777d
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

<<<<<<< HEAD
std::map<std::string, std::string> HTTP::GetBodyParams() {
=======
std::unordered_map<std::string, std::string> HTTP::GetBodyParams() {
>>>>>>> 5fb75eee5bf7b656a9927332a5ba5a984378777d
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