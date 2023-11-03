#include <iostream>
#include <string>

using namespace std;

const std::string ENV[ 27 ] = {
    "CONTENT_TYPE",
    "CONTENT_LENGTH",
    "DOCUMENT_ROOT", 
    "GATEWAY_INTERFACE",
    "HTTP_REFERER",
    "HTTP_COOKIE",
    "HTTP_ACCEPT",
    "HTTP_ACCEPT_ENCODING",
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
    "SERVER_SIGNATURE",
    "SERVER_SOFTWARE"
};

int main() {
    cout << "Content-type:text/html\r\n\r\n";
    cout << "<html>\n";
    cout << "<head>\n";
    cout << "<title>CGI Environment Variables</title>\n";
    cout << "</head>\n"; 
    cout << "<body>\n";
    cout << "<table border = \"0\" cellspacing = \"2\">";
    
    for (int i = 0; i < 26; i++) {
        cout << "<tr><td>" << ENV[ i ] << "</td><td>";
        char *value = getenv( ENV[ i ].c_str() );
    
        if ( value ) {
            cout << value;
        } else {
            cout << "Environment variable does not exists.";
        }
    }

    cout << "</table>\n";
    cout << "</body>\n";
    cout << "</html>\n";

    return 0;
}