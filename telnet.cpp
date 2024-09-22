/*
gcc -c -fPIE telnet.cpp -o telnet.o
gcc telnet.o -o telnet -pie
chmod +x telnet
./telnet
test
ftp.dlptest.com 21
dlpuser
rNrKYTX9g7z3RgJRmxWuGHbeu
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <termios.h>
#include <termios.h>
#include <fcntl.h>
 #include<iostream>
#include<stdio.h>                // printf
#include<string.h>                // strlen
#include<string>                // string
#include<sys/socket.h>            // socket
#include<arpa/inet.h>            // inet_addr
#include<netdb.h>                // hostenta
#include <unistd.h>
#include<iostream>
#include<sys/socket.h>            // socket
#include<arpa/inet.h>            // inet_addr
#include <sys/time.h> 
//using namespace std;
#define BUFLEN 20
int len;

 unsigned char buf[BUFLEN + 1];
struct sockaddr_in server;
/*telnet*/
#define DO 0xfd
#define WONT 0xfc
#define WILL 0xfb
#define DONT 0xfe
#define CMD 0xff
#define CMD_ECHO 1
#define CMD_WINDOW_SIZE 31
#define BUFLEN 20
#define IAC 255
#define CMD 0xff
#define DONT 0xfe
#define DO 0xfd
#define WONT 0xfc
#define WILL 0xfb
#define SB 0xfa
#define GA 0xf9
#define EL 0xf8
#define EC 0xf7
#define AYT 0xf6
#define AO 0xf5
#define IP 0xf4
#define BREAK 0xf3
#define SYNCH 0xf2
#define NOP 0xf1
#define SE 0xf0
#define EOR     0xef
#define ABORT 0xee
#define SUSP 0xed
#define xEOF 0xec
//#define BUFLEN 512
#define PRELIM 1
#define COMPLETE 2
#define CONTINUE 3
#define TRANSIENT 4
#define ERROR 5
#define RRQ 01
#define WRQ 02
#define DATA 03
#define ACK 04
#define REC_ESC '\377'
#define REC_EOR '\001'
#define REC_EOF '\002'
#define BLK_EOR 0x80
#define BLK_EOF 0x40
#define BLK_ERRORS 0x20
#define BLK_RESTART 0x10
static struct termios tin;

// Konwersja hosta na IP
char* resolve_hostname(char* hostname) {
    struct hostent* he;
    struct in_addr** addr_list;

    printf("Rozwiązywanie nazwy hosta: %s\n", hostname);

    if ((he = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname");
        return NULL;
    }

    addr_list = (struct in_addr**) he->h_addr_list;
    if (addr_list[0] != NULL) {
        printf("Rozwiązano IP: %s\n", inet_ntoa(*addr_list[0]));
        return inet_ntoa(*addr_list[0]);
    }
    return NULL;
}

void negotiate2(int sock, unsigned char *buf, int len) {
    int i;
   
    if (buf[1] == DO && buf[2] == CMD_WINDOW_SIZE) {
        unsigned char tmp1[10] = {255, 251, 31};
        if (send(sock, tmp1, 3 , 0) < 0)
            exit(1);
       
        unsigned char tmp2[10] = {255, 250, 31, 0, 80, 0, 24, 255, 240};
        if (send(sock, tmp2, 9, 0) < 0)
            exit(1);
        return;
    }
   
    for (i = 0; i < len; i++) {
        if (buf[i] == DO)
            buf[i] = WONT;
        else if (buf[i] == WILL)
            buf[i] = DO;
    }

    if (send(sock, buf, len , 0) < 0)
        exit(1);
}

void negotiate(int sock, unsigned char *buf, int len) {
    for (int i = 0; i < len; i += 3) {
        if (buf[i] != IAC) continue; // Sprawdzamy, czy jest to komenda Telnetu

        unsigned char option = buf[i+1]; // Druga wartość to opcja DO, DONT, WILL, WONT
        unsigned char command = buf[i+2]; // Trzecia wartość to konkretna opcja, np. ECHO

        if (option == DO) {
            // Odrzucamy wszystkie opcje DO (nie chcemy ich obsługiwać)
            unsigned char response[3] = { IAC, WONT, command };
            send(sock, response, 3, 0);
        } else if (option == DONT) {
            // Potwierdzamy, że nie będziemy robić tego, o co prosi serwer
            unsigned char response[3] = { IAC, WONT, command };
            send(sock, response, 3, 0);
        } else if (option == WILL) {
            // Odrzucamy wszystkie opcje WILL (nie chcemy ich obsługiwać)
            unsigned char response[3] = { IAC, DONT, command };
            send(sock, response, 3, 0);
        } else if (option == WONT) {
            // Potwierdzamy, że serwer nie będzie tego robił
            unsigned char response[3] = { IAC, DONT, command };
            send(sock, response, 3, 0);
        }
    }
}
static void terminal_set(void) {
    // save terminal configuration
    tcgetattr(STDIN_FILENO, &tin);
   
    static struct termios tlocal;
    memcpy(&tlocal, &tin, sizeof(tin));
    cfmakeraw(&tlocal);
    tcsetattr(STDIN_FILENO,TCSANOW,&tlocal);
}

static void terminal_reset(void) {
    // restore terminal upon exit
    tcsetattr(STDIN_FILENO,TCSANOW,&tin);
}

int main(int argc, char* argv[]) {
     char hostname[50];
    int port;

    if (argc < 3) {
        printf("Podaj nazwę hosta np. wp.pl, google.pl, ftp.dlptest.com  : \n");
        printf("host>>\n");
        if (fgets(hostname, sizeof(hostname), stdin) == NULL) {
            perror("fgets");
            return 1;
        }
        hostname[strcspn(hostname, "\n")] = '\0'; // Usuwamy znak nowej linii

        printf("Podaj port np. 21, 80, 443, 8080, 65535 ");
        printf("port>>\n");
        if (scanf("%d", &port) != 1) {
            printf("Błędny format portu\n");
            return 1;
        }

        // Walidacja portu
        if (port < 1 || port > 65535) {
            printf("Port musi być liczbą z zakresu 1-65535\n");
            return 1;
        }
    } else {
        // Jeśli podano argumenty wiersza poleceń, użyj ich
   //     hostname = argv[1];
        port = atoi(argv[2]);
    }
  struct timeval ts;
    ts.tv_sec = 1;  // Ustawienie timeoutu na 1 sekundę
    ts.tv_usec = 0; 
    // Sprawdzenie, czy port jest dozwolony (omijanie portów 80 i 443)


    // Rozwiązywanie nazwy hosta na IP
    char* ip_address = resolve_hostname(hostname);
    if (ip_address == NULL) {
        printf("Nie udało się rozwiązać hosta.\n");
        return 1;
    }

    // Tworzenie socketu
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Nie można utworzyć socketu\n");
        return 1;
    }

    // Konfiguracja serwera
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip_address);
    server.sin_port = htons(port);

    // Łączenie z serwerem
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Błąd połączenia");
        close(sock);
        return 1;
    }

    printf("Połączono z serwerem %s na porcie %d\n", ip_address, port);

    // Inicjalizacja zmiennych dla select()
    fd_set fds;
    struct timeval timeout;
    timeout.tv_sec = 1;  // 1 sekunda
    timeout.tv_usec = 0;

    // Otwieranie pliku do zapisu
    FILE* fp = fopen("telnet_output.txt", "w");
    if (fp == NULL) {
        printf("Błąd otwierania pliku\n");
        close(sock);
        return 1;
    }

  // Wstawiamy funkcję negotiate w odpowiednie miejsce w kodzie
while (1) {
	
  // terminal_set();
    atexit(terminal_reset);  
	
    fd_set fds;
    /* Set up polling. */
    FD_ZERO(&fds);
    if (sock != 0)
        FD_SET(sock, &fds);
    FD_SET(0, &fds);

    // wait for data
    int nready = select(sock + 1, &fds, (fd_set *) 0, (fd_set *) 0, &ts);
    if (nready < 0) {
        perror("select. Error");
        return 1;
    }
    else if (nready == 0) {
        ts.tv_sec = 1; // 1 second
        ts.tv_usec = 0;
    }
    else if (sock != 0 && FD_ISSET(sock, &fds)) {
        // start by reading a single byte
        int rv;
        if ((rv = recv(sock, buf, 1, 0)) < 0) {
            return 1;
        }
        else if (rv == 0) {
            printf("Connection closed by the remote end\n\r");
            return 0;
        }

        if (buf[0] == IAC) {
            // Odczytujemy 2 dodatkowe bajty i negocjujemy
            len = recv(sock, buf + 1, 2, 0);
            if (len < 0) {
                return 1;
            } else if (len == 0) {
                printf("Connection closed by the remote end\n\r");
                return 0;
            }
            // Wywołanie funkcji negotiate
            negotiate(sock, buf, 3);
        } else {
            len = 1;
            buf[len] = '\0';
            printf("%s", buf);
            fprintf(fp, "%s", buf);
            fflush(0);
        }
    } else if (FD_ISSET(0, &fds)) {
        buf[0] = getc(stdin); //fgets(buf, 1, stdin);
        if (send(sock, buf, 1, 0) < 0)
            return 1;
        if (buf[0] == '\n') // with the terminal in raw mode we need to force a LF
            putchar('\r');
    }
}


    fclose(fp);
    close(sock);
    return 0;
}
