#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *query = getenv("QUERY_STRING");

    printf("Content-type: text/html\n\n");
    printf("<html>\n");
    printf("<head>\n");
    printf("<title>Hello from CGI!</title>\n");
    printf("</head>\n");
    printf("<body>\n");
    printf("<h1>Hello, World!</h1>\n");

    // Sprawdzenie, czy przekazano parametr "name"
    if (query != NULL) {
        char *name = strstr(query, "name=");
        if (name != NULL) {
            name += 5; // Przesunięcie wskaźnika za "name="
            printf("<p>Witaj, %s!</p>", name);
        }
    }

    printf("</body>\n");
    printf("</html>\n");

    return 0;
}