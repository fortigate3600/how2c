#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SEGMENTSIZE sizeof(sem_t)
#define SEGMENTPERM 0666

int main(int argc,char** argv){
    sem_t *sp;
    int retval, id, err;
    /*Get shared memory segment*/
    id = shmget(IPC_PRIVATE, SEGMENTSIZE, SEGMENTPERM);
    if (id == -1) perror("Creation");
    else printf("Allocated %d\n", id);
    sp = (sem_t *)shmat(id ,(void *)0, 0); /*Attach segment.*/
    if ((long)sp == -1) {
        perror("Attachment.");
        exit(2);
    }
    /* Initialize the semaphore. */
    retval=sem_init(sp, 1, 2);//1 perche è per i processi, 2 il valore del sem
    if (retval != 0) {
        perror("Couldn’t initialize.");
        exit(3);
    }

    retval=sem_trywait(sp);
    printf("Did trywait. Returned %d>\n", retval);
    getchar ();
    retval=sem_trywait(sp);
    printf("Did trywait. Returned %d>\n", retval);
    getchar ();
    retval=sem_trywait(sp);
    printf("Did trywait. Returned %d>\n", retval);
    getchar ();
    sem_destroy(sp);
    err=shmctl(id, IPC_RMID, 0); /* Remove segment */
    if (err == -1) perror("Removal.");
    else printf("Removed. %d\n", err);
    return 0;
}