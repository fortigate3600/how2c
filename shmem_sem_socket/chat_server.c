#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Processo figlio: riceve messaggi
        char buffer[BUFFER_SIZE];
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = read(client_socket, buffer, BUFFER_SIZE - 1);
            if (bytes_received <= 0) {
                perror("Errore nella ricezione");
                break;
            }
            printf("Client: %s\n", buffer);

            if (strncmp(buffer, "QUIT", 4) == 0) {
                printf("Client ha chiuso la connessione.\n");
                break;
            }
        }
        close(client_socket);
        exit(0);
    } else {
        // Processo padre: invia messaggi
        char buffer[BUFFER_SIZE];
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            printf("Server: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0'; // Rimuove il newline

            if (send(client_socket, buffer, strlen(buffer), 0) <= 0) {
                perror("Errore nell'invio");
                break;
            }

            if (strncmp(buffer, "QUIT", 4) == 0) {
                printf("Server ha chiuso la connessione.\n");
                kill(pid, SIGKILL); // Termina il processo figlio
                break;
            }
        }
        close(client_socket);
    }
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Errore nella creazione della socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Errore nel binding");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Errore nella listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server in ascolto sulla porta %d...\n", PORT);

    if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Errore nell'accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connessione accettata.\n");
    handle_client(client_socket);

    close(server_fd);
    return 0;
}
