/* cgi_hello.cpp public domain NB: tested code. */
/*
g++ -c -fPIE cgi_hello.cpp -o cgi_hello.cgi -pie
g++ cgi_hello.cpp -o cgi_hello.cgi -pie
chmod +x cgi_hello.cgi
./cgi_hello.cgi
*/
#include <iostream>

using namespace std;

int main()
{
	//cout << "Content-type: text/html\n";
	cout << "HTTP/1.1 200 OK\r\n";
	cout << "Content-Type: text/html\r\n"; 
	cout << "\r\n";
	cout << "\n";	



	cout << "<!doctype html>\n";
	cout << "<html>\n";
	cout << "<title>Hello</title>\n";
	cout << "<p>Hello, world!\n";
	// C strings can, with care, contain double-quote characters, newlines,
	// etc.
	cout << "<p><a href=\"http://validator.w3.org/check?uri=referer\">";
	cout << "Valid.</a>\n";
	cout << "</html>\n";
	cout << endl;



	//send_CGI_header();
//	send_contents();
	return 0;
}
