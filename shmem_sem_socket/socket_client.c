#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Creazione del socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Errore nella creazione della socket");
        exit(EXIT_FAILURE);
    }

    // Configurazione dell'indirizzo del server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Conversione dell'indirizzo IP
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Indirizzo non valido o non supportato");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Connessione al server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Errore nella connessione");
        close(sock);
        exit(EXIT_FAILURE);
    }
    
    // Invio del messaggio al server
    const char *message = "Ciao, server!";
    send(sock, message, strlen(message), 0);
    printf("Messaggio inviato: %s\n", message);

    // Ricezione della risposta
    int valread = read(sock, buffer, BUFFER_SIZE);
    printf("Risposta ricevuta dal server: %s\n", buffer);

    // Chiusura socket
    close(sock);
    return 0;
}
