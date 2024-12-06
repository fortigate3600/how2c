#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024 // Dimensione della memoria condivisa

int main(int argc, char **argv) {
    int shm_id;
    int *shm_ptr;

    // Creazione o recupero del segmento di memoria condivisa
    shm_id = shmget(IPC_PRIVATE, SHM_SIZE, 0666);
    if (shm_id < 0) {
        perror("Errore nella creazione della memoria condivisa");
        exit(1);
    }
    printf("id: %d\n",shm_id);

    // Collegamento al segmento di memoria condivisa
    shm_ptr = (int *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (int *)-1) {
        perror("Errore nel collegamento alla memoria condivisa");
        exit(1);
    }

    // Lettura del valore iniziale
    printf("Valore iniziale nella memoria condivisa: %d\n", *shm_ptr);

    // Scrittura di un nuovo valore
    *shm_ptr = 42; // Scrive il valore 42 nella memoria condivisa
    printf("Nuovo valore scritto nella memoria condivisa: %d\n", *shm_ptr);

    // Scollegamento dal segmento di memoria condivisa
    if (shmdt((void *)shm_ptr) == -1) {
        perror("Errore nello scollegamento dalla memoria condivisa");
        exit(1);
    }
    printf("Memoria condivisa scollegata correttamente.\n");

    // Rimozione del segmento
    if (shmctl(shm_id, IPC_RMID, 0) == -1) {
        perror("Errore shmctl IPC_RMID");
    }
    printf("Memoria condivisa eliminata.\n");
    return 0;
}
