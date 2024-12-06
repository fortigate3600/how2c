#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

// Bidirectional communication between parent and child
int main1(){
    int pipe_fdp[2]; // Pipe dal padre al figlio
    int pipe_fdf[2]; // Pipe dal figlio al padre
    pid_t pid;
    char write_msg[] = "Messaggio inviato dal processo padre!";
    char read_msg[100];

    // Creazione delle pipe
    if (pipe(pipe_fdp) == -1 || pipe(pipe_fdf) == -1) {
        perror("Errore nella creazione delle pipe");
        exit(1);
    }

    // Creazione del processo figlio
    if ((pid = fork()) < 0) {
        perror("Errore nella creazione del processo figlio");
        exit(1);
    } 

    if (pid == 0) { // Codice del processo figlio
        close(pipe_fdp[1]); // Chiude il lato di scrittura della pipe 1 (padre -> figlio)
        close(pipe_fdf[0]); // Chiude il lato di lettura della pipe 2 (figlio -> padre)

        // Legge il messaggio dal padre
        read(pipe_fdp[0], read_msg, sizeof(read_msg));
        printf("FIGLIO ha letto: %s\n", read_msg);

        // Invia una risposta al padre
        strcpy(write_msg, "Messaggio ricevuto dal figlio");
        write(pipe_fdf[1], write_msg, strlen(write_msg) + 1);

        close(pipe_fdp[0]);
        close(pipe_fdf[1]);
        exit(0);
    } else { // Codice del processo padre
        close(pipe_fdp[0]); // Chiude il lato di lettura della pipe 1
        close(pipe_fdf[1]); // Chiude il lato di scrittura della pipe 2

        // Invia un messaggio al figlio
        write(pipe_fdp[1], write_msg, strlen(write_msg) + 1);

        // Legge la risposta del figlio
        read(pipe_fdf[0], read_msg, sizeof(read_msg));
        printf("PADRE ha letto: %s\n", read_msg);

        waitpid(pid, NULL, 0);
        close(pipe_fdp[1]);
        close(pipe_fdf[0]);
    }

    return 0;
}

// Parent process sends a list; child returns normalized numbers
int main2(){
    int pipe_fdp[2]; // Pipe dal padre al figlio
    int pipe_fdf[2]; // Pipe dal figlio al padre
    pid_t pid;

    // Creazione delle pipe
    if (pipe(pipe_fdp) == -1 || pipe(pipe_fdf) == -1) {
        perror("Errore nella creazione delle pipe");
        exit(1);
    }

    // Creazione del processo figlio
    if ((pid = fork()) < 0) {
        perror("Errore nella creazione del processo figlio");
        exit(1);
    } 

    if (pid == 0) { // Codice del processo figlio
        close(pipe_fdp[1]); // Chiude il lato di scrittura della pipe 1 (padre -> figlio)
        close(pipe_fdf[0]); // Chiude il lato di lettura della pipe 2 (figlio -> padre)

        // Legge il messaggio dal padre
        int recvnumbers[5];
        read(pipe_fdp[0], recvnumbers, sizeof(recvnumbers));

        printf("FIGLIO: ho letto i numeri ricevuti\n");
        int sum=0;
        for(int i=0;i<5;i++){
            printf("FIGLIO: numero ricevuto %d: %d\n",i,recvnumbers[i]);
            sum+=recvnumbers[i];
        }

        double sendnumbers[5];
        for(int i=0;i<5;i++){
            sendnumbers[i] = (double)recvnumbers[i]/sum;
            printf("FIGLIO: numero normalizzato %d: %f\n",i,sendnumbers[i]);
        }

        // Invia una risposta al padre
        write(pipe_fdf[1], sendnumbers, sizeof(sendnumbers));

        close(pipe_fdp[0]);
        close(pipe_fdf[1]);
        exit(0);
    } else { // Codice del processo padre
        close(pipe_fdp[0]); // Chiude il lato di lettura della pipe 1
        close(pipe_fdf[1]); // Chiude il lato di scrittura della pipe 2

        // Invia un messaggio al figlio
        int sendnumbers[] = {1,2,3,5,10};
        write(pipe_fdp[1], sendnumbers, sizeof(sendnumbers));

        // Legge la risposta del figlio
        double recvnumbers[5];
        int ret=read(pipe_fdf[0], recvnumbers, sizeof(recvnumbers));
        printf("PADRE: ho letto i numeri ricevuti ret: %d\n",ret);

        for(int i=0;i<5;i++){
            printf("PADRE: numero ricevuto %d: %f\n",i,recvnumbers[i]);
        }

        waitpid(pid, NULL, 0);
        close(pipe_fdp[1]);
        close(pipe_fdf[0]);
    }

    return 0;
}

// 5 children receive data from the parent through a single pipe
int main3(){
    int pipe_fd[2]; // Pipe dal padre al figlio
    pid_t pid[5];
    char write_msg[] = "Messaggio inviato dal processo padre!";
    char read_msg[100];

    // Creazione delle pipe
    if (pipe(pipe_fd) == -1) {
        perror("Errore nella creazione delle pipe");
        exit(1);
    }

    // Creazione dei processi figli
    int i=0;
    while (i < 5) {
        pid[i] = fork();
        if (pid[i] < 0) {
            perror("Errore nella creazione del processo figlio");
            exit(1);
        }
        if (pid[i] == 0) { // Codice processo figlio
            close(pipe_fd[1]); // Chiude il lato di scrittura

            // Legge il messaggio dal padre
            read(pipe_fd[0], read_msg, sizeof(read_msg));
            printf("FIGLIO %d, processo: %d, ho letto: %s\n", getpid()-getppid(), getpid(), read_msg);

            close(pipe_fd[1]);
            exit(0);
        }
        i++;   
    } // PADRE:
    close(pipe_fd[0]);

    // Invia un messaggio ai figli
    for (int i = 0; i < 5; i++) {
        // Scrive il messaggio nella pipe per il figlio successivo
        write(pipe_fd[1], write_msg, strlen(write_msg) + 1);

        // Attende che il figlio termini
        waitpid(pid[i], NULL, 0);
    }
    close(pipe_fd[1]);
    return 0;
}

int main(){
    printf("1 for Bidirectional communication between parent and child\n2 for Parent process sends a list; child returns normalized numbers\n3 for 5 children receive data from the parent through a single pipe\n\nWhich test do you want to run: 1, 2 or 3? ");
    char c=getchar();
    if(c=='1') main1();
    else if(c=='2') main2();
    else if(c=='3') main3();
    else printf("cazzo non era difficile\n");
    return 0;
}