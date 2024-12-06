#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define SHM_SIZE 1024       // Dimensione della memoria condivisa
#define SEM_KEY 1234        // Chiave del semaforo

// Operazioni semaforiche
struct sembuf sem_lock = {0, -1, 0}; // Decrementa il semaforo (lock)
struct sembuf sem_unlock = {0, 1, 0}; // Incrementa il semaforo (unlock)

int main() {
    int shm_id, sem_id;
    char *shm_ptr;

    // Creazione della memoria condivisa
    shm_id = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Errore nella creazione della memoria condivisa");
        exit(1);
    }

    // Collegamento alla memoria condivisa
    shm_ptr = (char *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (char *)-1) {
        perror("Errore nel collegamento alla memoria condivisa");
        exit(1);
    }

    // Creazione del semaforo
    sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("Errore nella creazione del semaforo");
        exit(1);
    }

    // Inizializzazione del semaforo a 0 (bloccato)
    if (semctl(sem_id, 0, SETVAL, 0) == -1) {
        perror("Errore nell'inizializzazione del semaforo");
        exit(1);
    }

    // Creazione del processo figlio
    pid_t pid = fork();

    if (pid == 0) { // Processo figlio
        printf("Processo figlio: attendo che il semaforo sia sbloccato...\n");

        // Attende che il semaforo sia sbloccato (lock)
        if (semop(sem_id, &sem_lock, 1) == -1) {
            perror("Errore nel lock del semaforo");
            exit(1);
        }

        // Legge dalla memoria condivisa
        printf("Processo figlio: stringa letta dalla memoria condivisa: \"%s\"\n", shm_ptr);

        // Sblocca il semaforo per il processo padre
        if (semop(sem_id, &sem_unlock, 1) == -1) {
            perror("Errore nello sblocco del semaforo");
            exit(1);
        }

        // Scollegamento dalla memoria condivisa
        shmdt(shm_ptr);
        exit(0);
    } else { // Processo padre
        // Scrive nella memoria condivisa
        strcpy(shm_ptr, "Ciao dal processo padre!");
        printf("Processo padre: stringa scritta nella memoria condivisa.\n");

        // Sblocca il semaforo per il processo figlio
        if (semop(sem_id, &sem_unlock, 1) == -1) {
            perror("Errore nello sblocco del semaforo");
            exit(1);
        }

        // Attende il processo figlio
        wait(NULL);

        // Rimuove il semaforo
        if (semctl(sem_id, 0, IPC_RMID) == -1) {
            perror("Errore nella rimozione del semaforo");
        }

        // Rimuove la memoria condivisa
        shmctl(shm_id, IPC_RMID, NULL);

        printf("Processo padre: semaforo e memoria condivisa rimossi.\n");
    }

    return 0;
}
