#include <fcntl.h>    // per open, O_CREAT, O_WRONLY, O_RDONLY
#include <unistd.h>   // per write, read, close
#include <stdlib.h>   // per exit
#include <string.h>
#include <stdio.h>
#define PATHFILE "nomefile.txt"

int main() {
    char text[100] = "Hello World! Linea 0\n";

    // Aprire il file in modalità scrittura, creandolo se non esiste
    int fd = open(PATHFILE, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fd == -1) {
        perror("Errore nell'apertura o creazione del file");
        exit(EXIT_FAILURE);
    }

    // Scrivere la stringa nel file
    if(write(fd,text,strlen(text)) == -1) {
        perror("Errore nella scrittura del file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Scrivere n righe nel testo
    int n=10;
    for (int i=0;i<n;i++) {
        memset(text, 0, sizeof(text));
        sprintf(text, "Questa è una riga di esempio, Linea %d\n", i+1);

        if (write(fd, text, strlen(text)) == -1) {
            perror("Errore nella scrittura del file");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }
    
    // Chiudere e riaprire il file dopo la scrittura per la lettura altrimenti non si sincronizza
    if (close(fd) == -1) {
        perror("Errore nella chiusura del file dopo la scrittura");
        exit(EXIT_FAILURE);
    }
    fd = open(PATHFILE, O_RDONLY);
    if (fd == -1) {
        perror("Errore nell'apertura del file per la lettura");
        exit(EXIT_FAILURE);
    }

    // Leggere il file
    char buffer[1024];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1); // lasciare spazio per il terminatore null
    if (bytes_read == -1) {
        perror("Errore nella lettura del file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Aggiungere il terminatore di stringa
    buffer[bytes_read] = '\0';

    // oppure
    // letture byte per byte
    char *buffer2 = NULL;
    char byte;
    size_t buffer_size = 0;
    int ret;
    lseek(fd, 0, SEEK_SET);//riporto la lettura all'inizio del file

    while ((ret = read(fd, &byte, 1)) > 0) {
        buffer2 = realloc(buffer2, buffer_size + 1);
        buffer2[buffer_size] = byte;
        buffer_size++;
    }
    // Aggiungere il terminatore di stringa
    buffer2 = realloc(buffer2, buffer_size + 1);
    buffer2[buffer_size] = '\0';


    // Stampare il contenuto del file
    printf("Contenuto letto dal file:\n%s", buffer);
    printf("\nContenuto letto dal file (secondo metodo):\n%s", buffer2);

    // Chiudere il file
    if (close(fd) == -1) {
        perror("Errore nella chiusura del file dopo la lettura");
        exit(EXIT_FAILURE);
    }
    free(buffer2);
    return 0;
}
