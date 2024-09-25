#include <stdio.h>


#define MAX_HOST_LEN 100

int main() {
    char host[MAX_HOST_LEN];
    int port;

    printf("Podaj nazwę hosta: ");
    scanf("%99s", host); // Ograniczamy długość wejścia do 99 znaków

    printf("Podaj numer portu: ");
    if (scanf("%d", &port) != 1) {
        printf("Błąd podczas wprowadzania portu.\n");
        return 1;
    }

    printf("Podany host: %s\n", host);
    printf("Podany port: %d\n", port);

    return 0;
}