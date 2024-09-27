#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>  // Dodane do obsługi katalogów

#define PORT 8080
#define BUFFER_SIZE 2024
#define MAX_FILE_SIZE 2024

/* CGI handler */
void handle_cgi_request(int client_socket, char *cgi_path) {
    pid_t pid = fork();
    if (pid == 0) {  // Child process
        close(1);  // Zamknij stdout
        dup2(client_socket, 1);  // Przekieruj stdout na socket klienta
char *argv[] = {"sh", "-c", NULL};
        //char *argv[] = {cgi_path, NULL};
        setenv("QUERY_STRING", "", 1);  // Możesz tu przekazać zapytania GET

        execv(cgi_path, argv);  // Uruchom CGI
        perror("execv failed");
        exit(1);  // Zakończ jeśli execv się nie uda
    } else if (pid > 0) {
        //waitpid(pid, NULL, 0);  // Czekaj na zakończenie procesu potomnego
    } else {
        perror("fork failed");
    }
}

/* Odpowiedź HTTP */
void send_response(int connfd, const char *status, const char *content_type, const char *body) {
    char response[1024];
    snprintf(response, sizeof(response), "HTTP/1.1 %s\r\nContent-Type: %s\r\n\r\n%s",
             status, content_type, body);
    send(connfd, response, strlen(response), 0);
}

/* Ładowanie plików HTML */
char *load_file(const char *file_path) {
    FILE *fp = fopen(file_path, "r");
    if (!fp) {
        perror("Failed to open file");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = (char *)malloc(file_size + 1);
    if (!buffer) {
        perror("Memory allocation failed");
        fclose(fp);
        return NULL;
    }

    fread(buffer, 1, file_size, fp);
    buffer[file_size] = '\0';
    fclose(fp);
    
    return buffer;
}

/* Uruchamianie komend systemowych */
char *run_command(const char *cmd) {
    char *output = (char *)malloc(MAX_FILE_SIZE);
    if (!output) {
        perror("Memory allocation failed");
        return NULL;
    }

    FILE *fp = popen(cmd, "r");
    if (!fp) {
        perror("popen failed");
        free(output);
        return NULL;
    }

    size_t len = fread(output, 1, MAX_FILE_SIZE - 1, fp);
    output[len] = '\0';

    pclose(fp);
    return output;
}

/* Obsługa POST */
void handle_post(int client_socket, const char *data) {
    printf("Received POST data: %s\n", data);
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 24\r\n"
        "\r\n"
        "Hello, POST!";
    send(client_socket, response, strlen(response), 0);
}
/* Tworzenie dynamicznego pliku HTML z wynikami */
void create_console_html(const char *msg) {
    FILE *fp = fopen("index.html", "w");
    if (fp == NULL) {
        perror("Failed to create index.html");
        return;
    }

    fprintf(fp,
            "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Simple Console</title></head><body>"
            "<h1>Simple Server Console</h1>"
            "<form method=\"POST\" action=\"\">"
            "<label for=\"command\">Enter command:</label><br>"
            "<input type=\"text\" id=\"command\" name=\"command\" size=\"50\"><br><br>"
            "<input type=\"submit\" value=\"Execute\"></form>"
            "<h2>Output:</h2><pre id=\"output\">%s</pre></body></html>", msg);

    fclose(fp);
}
/* Obsługa GET */
void handle_get(int client_socket, const char *uri) {
    char *page = (char *)malloc(1024);
    if (!page) {
        send_response(client_socket, "500 Internal Server Error", "text/plain", "Memory allocation failed");
        return;
    }

    // Domyślna strona główna
    if (strcmp(uri, "/") == 0) {
        char *html = load_file("index.html");
        if (html) {
            send_response(client_socket, "200 OK", "text/html", html);
            free(html);
        } else {
            send_response(client_socket, "404 Not Found", "text/plain", "File not found");
        }
    }
    // Obsługa plików CGI
    else if (strstr(uri, ".cgi")) {
        char cgi_path[256];
        snprintf(cgi_path, sizeof(cgi_path), "./%s", uri);  // Generowanie ścieżki do pliku CGI
      
        handle_cgi_request(client_socket, cgi_path);
        
    }
    // Obsługa innych plików HTML
    else {
        snprintf(page, 2024, ".%s", uri);
        char *html = load_file(page);
        if (html) {
            send_response(client_socket, "200 OK", "text/html", html);
            free(html);
        } else {
            send_response(client_socket, "404 Not Found", "text/plain", "File not found");
        }
    }

    free(page);
}

/* Parsowanie zapytań */
void parse_request_line(char *req, char *method, char *uri) {
    sscanf(req, "%s %s", method, uri);
}



/* Główna funkcja serwera */
int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    printf("Server is running on https://127.0.0.1:8080\n");

    // Tworzenie socketu
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bindowanie socketu
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Nasłuchiwanie na porcie
    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    create_console_html("Ready");

    // Główna pętla serwera
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        int received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (received < 0) {
            perror("recv failed");
            close(client_socket);
            continue;
        }
        buffer[received] = '\0';  // Null-terminate the received data

        char method[20], uri[1024];
        parse_request_line(buffer, method, uri);

        if (strcmp(method, "GET") == 0) {
            handle_get(client_socket, uri);
        } else if (strcmp(method, "POST") == 0) {
            char *post_data = strstr(buffer, "\r\n\r\n");
            if (post_data) {
                post_data += 12;  // Skip the "\r\n\r\n"
                char *output = run_command(post_data);
                if (output) {
                    create_console_html(output);
                    free(output);
                }
            }
        }

        close(client_socket);
    }

    close(server_socket);
    return 0;
}
