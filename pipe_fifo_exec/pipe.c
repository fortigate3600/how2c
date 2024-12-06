#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int main() {
    printf("ao\n");
    int pipe_fd[2];
    pid_t pid;
    char write_msg[] = "Messaggio inviato dal processo figlio!";
    char read_msg[100];

    // Creazione della pipe
    if (pipe(pipe_fd) == -1) {
        perror("Errore nella creazione della pipe");
        exit(1);
    }

    // Creazione del processo figlio
    if ((pid = fork()) < 0) {
        perror("Errore nella creazione del processo figlio");
        exit(1);
    } else if (pid == 0) {
        // Codice del processo figlio
        close(pipe_fd[0]); // Chiudiamo il lato di lettura della pipe
        write(pipe_fd[1], write_msg, strlen(write_msg) + 1);
        close(pipe_fd[1]); // Chiudiamo il lato di scrittura dopo l'invio
        exit(0);
    } else {
        // Codice del processo padre
        close(pipe_fd[1]); // Chiudiamo il lato di scrittura della pipe
        read(pipe_fd[0], read_msg, sizeof(read_msg));
        printf("Processo padre ha letto: %s\n", read_msg);
        close(pipe_fd[0]); // Chiudiamo il lato di lettura dopo la ricezione
    }

    return 0;
}
