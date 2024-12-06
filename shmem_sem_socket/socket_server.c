#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    close(server_fd);
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    
    // Creazione del socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Errore nella creazione della socket");
        exit(EXIT_FAILURE);
    }

    // Configurazione dell'indirizzo
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding della socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Errore nel binding");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Messa in ascolto
    if (listen(server_fd, 3) < 0) {
        perror("Errore nella listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server in ascolto sulla porta %d...\n", PORT);

    // Accettazione di una connessione
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Errore nell'accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    // Ricezione del messaggio
    int valread = read(new_socket, buffer, BUFFER_SIZE);
    printf("Messaggio ricevuto: %s\n", buffer);

    // Risposta al client
    const char *response = "Messaggio ricevuto dal server!";
    send(new_socket, response, strlen(response), 0);
    printf("Risposta inviata al client.\n");

    // Chiusura socket
    close(new_socket);
    close(server_fd);
    return 0;
}
