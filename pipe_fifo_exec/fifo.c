#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FIFO_NAME "my_fifo"
#define FIFO_NAME1 "my_fifo1"
#define FIFO_NAME2 "my_fifo2"
#define BUFFER_SIZE 128

// esempio
int main1() {
    unlink(FIFO_NAME);
    pid_t pid;
    int fd;
    char *message = "Hello from the writer process!\n";
    char buffer[BUFFER_SIZE];

    // Creazione della FIFO
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    // Creazione del processo figlio
    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Processo figlio (Lettore)
        printf("Reader process started...\n");
        
        // Apertura della FIFO per lettura
        fd = open(FIFO_NAME, O_RDONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        // Lettura del messaggio dalla FIFO
        if (read(fd, buffer, BUFFER_SIZE) > 0) {
            printf("Reader received: %s", buffer);
        }

        close(fd);
        printf("Reader process exiting...\n");
    } else {
        // Processo padre (Scrittore)
        printf("Writer process started...\n");

        // Apertura della FIFO per scrittura
        fd = open(FIFO_NAME, O_WRONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        // Scrittura del messaggio nella FIFO
        write(fd, message, strlen(message) + 1);
        printf("Writer sent: %s", message);

        close(fd);
        printf("Writer process exiting...\n");

        // Aspetta il processo figlio
        wait(NULL);
    }

    // Rimuovi la FIFO
    unlink(FIFO_NAME);

    return 0;
}

// comunicazione bidirezionale
int main2() {
    unlink(FIFO_NAME1);
    unlink(FIFO_NAME2);
    pid_t pid;
    int fd1, fd2;
    char *message = "Hello from the writer process!\n";
    char buffer[BUFFER_SIZE];

    // Creazione della FIFO
    if (mkfifo(FIFO_NAME1, 0666) == -1) {
        perror("mkfifo1");
        exit(EXIT_FAILURE);
    }

    if (mkfifo(FIFO_NAME2, 0666) == -1) {
        perror("mkfifo2");
        exit(EXIT_FAILURE);
    }

    // Creazione del processo figlio
    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf("FIGLIO: started\n");
        // Processo figlio (Lettore)
        fd1 = open(FIFO_NAME1, O_RDONLY);
        if (fd1 == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        // Lettura del messaggio dal padre
        if (read(fd1, buffer, BUFFER_SIZE) > 0) {
            printf("FIGLIO: received: %s", buffer);
        }

        fd2 = open(FIFO_NAME2, O_WRONLY);
        if (fd2 == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        write(fd2, message, strlen(message) + 1);
        printf("FIGLIO: sent: %s", message);

        close(fd1);
        close(fd2);
        printf("FIGLIO: process exiting...\n");
    } else {
        // Processo padre (Scrittore)
        fd1 = open(FIFO_NAME1, O_WRONLY);
        if (fd1 == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        // Scrittura del messaggio nella FIFO
        write(fd1, message, strlen(message) + 1);
        printf("PADRE: sent: %s", message);

        fd2 = open(FIFO_NAME2, O_RDONLY);
        if (fd2 == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        if (read(fd2, buffer, BUFFER_SIZE) > 0) {
            printf("PADRE: received: %s", buffer);
        }

        close(fd1);
        close(fd2);
        printf("PADRE: process exiting...\n");

        // Aspetta il processo figlio
        wait(NULL);
    }

    // Rimuovi la FIFO
    unlink(FIFO_NAME1);
    unlink(FIFO_NAME2);

    return 0;
}

// lettori multipli
int main() {
    unlink(FIFO_NAME);
    pid_t pid[5];
    int fd;
    char *message = "Hello from the writer process!\n";
    char buffer[BUFFER_SIZE];

    // Creazione della FIFO
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while (i++ < 5){
        pid[i] = fork();
        if (pid[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid[i] == 0) {
            // Processo figlio (Lettore)
            // Apertura della FIFO per lettura
            fd = open(FIFO_NAME, O_RDONLY | O_NONBLOCK);
            if (fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            printf("FIGLIO %d: Inizio lettura...\n",i);
            // Lettura del messaggio dalla FIFO
            int ret;
            do{
                ret=read(fd, buffer, BUFFER_SIZE);
                usleep(100);
                printf("FIGLIO %d: sono nel while, ret:%d\n", i, ret);
            } while(ret < 1);
            printf("ret: %d\n",ret);

            printf("FIGLIO %d: ho ricevuto: %s\n", i, buffer);
            close(fd);
            exit(EXIT_SUCCESS);
        }
    }

    // Processo padre (Scrittore)
    printf("PADRE: process started...\n");

    // Apertura della FIFO per scrittura
    fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Scrittura del messaggio nella FIFO
    write(fd, message, strlen(message) + 1);
    printf("Writer sent: %s", message);
    
    for (i=0; i<5; i++){
        wait(NULL);
        printf("PADRE: someone terminated\n");
        write(fd, message, strlen(message) + 1);
    }
    
    printf("tutti i processi hanno finito...\n");
    
    // Rimuovi la FIFO
    close(fd);
    unlink(FIFO_NAME);
    return 0;
}
