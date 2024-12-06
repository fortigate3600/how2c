#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>   

#define SHM_SIZE 1024

int main() {
    int shm_id;
    int *shm_ptr;

    // Creazione della memoria condivisa
    shm_id = shmget(IPC_PRIVATE, SHM_SIZE, 0666);
    if (shm_id < 0) {
        perror("Errore shmget");
        exit(1);
    }

    // Collegamento della memoria condivisa
    shm_ptr = (int *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (int *)-1) {
        perror("Errore shmat");
        exit(1);
    }

    // Scrittura iniziale
    *shm_ptr = 42;
    printf("Processo padre: valore iniziale = %d\n", *shm_ptr);

    // Creazione del processo figlio
    pid_t pid = fork();
    if (pid < 0) {
        perror("Errore fork");
        exit(1);
    }

    if (pid == 0) { // Processo figlio
        printf("Processo figlio: valore letto = %d\n", *shm_ptr);
        *shm_ptr = 84; // Modifica del valore nella memoria condivisa
        printf("Processo figlio: nuovo valore = %d\n", *shm_ptr);
        exit(0);
    } else { // Processo padre
        wait(NULL); // Aspetta il figlio
        printf("Processo padre: valore finale = %d\n", *shm_ptr);
    }

    // Scollegamento e rimozione della memoria condivisa
    shmdt((void *)shm_ptr);
    shmctl(shm_id, IPC_RMID, 0);

    return 0;
}
