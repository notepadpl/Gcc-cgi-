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
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <termios.h>

int len;

#define BUFLEN 512
#define IAC 255
#define DO 253
#define DONT 254
#define WILL 251
#define WONT 252
#define PASV 227
static struct termios tin;
 struct timeval ts;
struct Connection {
    char *ip;
    int port;
     char* hostname;
   
        char* ip_address;
} conn;

unsigned char buf[BUFLEN + 1];
struct sockaddr_in server;

char* resolve_hostname(char* hostname) {
    struct hostent* he;
    struct in_addr** addr_list;

    if ((he = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname");
        return NULL;
    }

    addr_list = (struct in_addr**) he->h_addr_list;
    if (addr_list[0] != NULL) {
        return inet_ntoa(*addr_list[0]);
    }
    return NULL;
}

void negotiate(int sock, unsigned char* buf, int len) {
    for (int i = 0; i < len; i += 3) {
        if (buf[i] != IAC) continue;
        unsigned char option = buf[i + 1];
        unsigned char command = buf[i + 2];

        if (option == DO) {
            unsigned char response[3] = { IAC, WONT, command };
            send(sock, response, 3, 0);
        }
    }
}

void handle_pasv_response(char* response) {
    int ip_parts[4], port_parts[2];

    sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)",
           &ip_parts[0], &ip_parts[1], &ip_parts[2], &ip_parts[3], &port_parts[0], &port_parts[1]);

    snprintf(conn.ip, sizeof(conn.ip), "%d.%d.%d.%d", ip_parts[0], ip_parts[1], ip_parts[2], ip_parts[3]);
    conn.port = (port_parts[0] << 8) + port_parts[1];
}
static void terminal_reset(void) {
    // restore terminal upon exit
    tcsetattr(STDIN_FILENO,TCSANOW,&tin);
}
int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Użycie: %s <adres_serwera> <port>\n", argv[0]);
        return 1;
    }
   
    conn.ip = argv[1];
    conn.port = atoi(argv[2]);
    /*prawie dobrze lco: cofa do poczatku programu nie tworzymy nowego polaczenoa tylko zmieniamy adres ip i port starego polaczenia */
 lco: 
 printf("jump! \n");

  char*  ip_address = resolve_hostname(conn.ip);
 
    if (ip_address == NULL) {
        printf("Nie udało się rozwiązać hosta.\n");
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Nie można utworzyć socketu\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip_address);
    server.sin_port = htons(conn.port);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Błąd połączenia");
        close(sock);
        return 1;
    }

    printf("Połączono z serwerem %s na porcie %d\n", ip_address, conn.port);

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
           // fprintf(fp, "%s", buf);
            fflush(0);
        }
           // Sprawdzenie, czy otrzymano komendę PASV
        if (buf[0]== PASV) {
            handle_pasv_response((char*)buf);
            close(sock);
            break;
            goto lco; // Przechodzimy do sekcji nawiązywania nowego połączenia
        }
    } else if (FD_ISSET(0, &fds)) {
        buf[0] = getc(stdin); //fgets(buf, 1, stdin);
        if (send(sock, buf, 1, 0) < 0)
            return 1;
        if (buf[0] == '\n') // with the terminal in raw mode we need to force a LF
            putchar('\r');
    }

     

   }

    close(sock);
    return 0;
}
