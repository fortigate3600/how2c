#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void chat_with_server(int sock) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Errore nella fork");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Processo figlio: riceve messaggi
        char buffer[BUFFER_SIZE];
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = read(sock, buffer, BUFFER_SIZE - 1);
            if (bytes_received <= 0) {
                perror("Errore nella ricezione");
                break;
            }
            printf("Server: %s\n", buffer);

            if (strncmp(buffer, "QUIT", 4) == 0) {
                printf("Server ha chiuso la connessione.\n");
                break;
            }
        }
        close(sock);
        exit(0);
    } else {
        // Processo padre: invia messaggi
        char buffer[BUFFER_SIZE];
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            printf("Client: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0'; // Rimuove il newline

            if (send(sock, buffer, strlen(buffer), 0) <= 0) {
                perror("Errore nell'invio");
                break;
            }

            if (strncmp(buffer, "QUIT", 4) == 0) {
                printf("Client ha chiuso la connessione.\n");
                kill(pid, SIGKILL); // Termina il processo figlio
                break;
            }
        }
        close(sock);
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Errore nella creazione della socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Indirizzo non valido o non supportato");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Errore nella connessione");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Connessione stabilita.\n");
    chat_with_server(sock);

    close(sock);
    return 0;
}
