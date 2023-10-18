#include <iostream>

using namespace std;

int main()
{
    cout << "Content-Type: text/html; charset=utf-8;" << endl << endl;
    cout << "<p>Привет!</p>" << endl;
    cout << "<p>Ваш IP-адрес: " << getenv("REMOTE_ADDR") << ".</p>" << endl;
    cout << "<p>Ваш браузер: " << getenv("HTTP_USER_AGENT") << ".</p>" << endl;
    return -1;
}
