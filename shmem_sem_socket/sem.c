#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

int *shared_counter;
sem_t *sem;

void process_function(int id) {
    for (int i = 0; i < 10; ++i) {
        sem_wait(sem); // Lock semaforo
        (*shared_counter)++;
        printf("Process %d incremented counter to %d\n", id, *shared_counter);
        sem_post(sem); // Unlock semaforo
        sleep(1); // Simula elaborazione
    }
}

int main() {
    // Allocare memoria condivisa
    shared_counter = mmap(NULL, sizeof *shared_counter, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem = mmap(NULL, sizeof *sem, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (shared_counter == MAP_FAILED || sem == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    *shared_counter = 0;

    // Inizializza semaforo
    if (sem_init(sem, 1, 1) == -1) { // Il secondo parametro (1) indica uso tra processi
        perror("sem_init failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Processo figlio
        process_function(1);
        exit(0);
    } else {
        // Processo padre
        process_function(2);
        wait(NULL); // Attendi il completamento del processo figlio
    }

    printf("Final counter value: %d\n", *shared_counter);

    // Distruggi semaforo e libera memoria condivisa
    if (sem_destroy(sem) == -1) {
        perror("sem_destroy failed");
    }
    munmap(shared_counter, sizeof *shared_counter);
    munmap(sem, sizeof *sem);

    return 0;
}

