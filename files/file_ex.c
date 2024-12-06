#include <fcntl.h>    // per open, O_CREAT, O_WRONLY, O_RDONLY
#include <unistd.h>   // per write, read, close
#include <stdlib.h>   // per exit
#include <string.h>
#include <stdio.h>

#define MAX_TOKEN 20
#define MAX_FILE 10

char* aproleggo(char* filename){
    char buffer[1024];
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Errore nell'apertura del file per la lettura");
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_read;
    bytes_read = read(fd, buffer, 1024);
    if (bytes_read == -1) {
        perror("Errore nella lettura del file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Aggiungere il terminatore di stringa
    buffer[bytes_read] = '\0';

    printf("Contenuto letto dal file:%s\n", buffer);

    if (close(fd) == -1) {
        perror("Errore nella chiusura del file dopo la lettura");
        exit(EXIT_FAILURE);
    }
    char* ret=malloc(bytes_read+1);
    strcpy(ret,buffer);
    return ret;
}
int main(int argc, char* argv[]) {

    char* buffer1 = malloc(1024);
    char* buffer2 = malloc(1024);
    char* buffer3 = malloc(1024);
    char* buffer4 = malloc(1024);
    

    char files[argc-1][10];
    for (int i = 1; i < argc; i++) {
        strcpy(files[i-1], argv[i]);
    }

    printf("files:\n");
    for(int i=0;i<argc-1;i++){
        printf("%s\n", files[i]);
    }
    
    buffer1=aproleggo(files[0]);
    buffer2=aproleggo(files[1]);


    char *token = strtok(buffer2, "\n");
    int j=0;
    char *arrayfile[MAX_TOKEN];
    while (token != NULL && j < MAX_TOKEN) {
        arrayfile[j] = token;
        j++;
        token = strtok(NULL, "\n"); 
    }
    int lenfiles=j;

    token = strtok(buffer1, "\n");
    int i=0;
    char *arraytoken[MAX_TOKEN];
    while (token != NULL && i < MAX_TOKEN) {
        arraytoken[i] = token;
        i++;
        token = strtok(NULL, "\n"); 
    }
    int lentoken=i;

    printf("file3 %s\n",arrayfile[0]);
    //apro i file in b.txt
    int fd3 = open(arrayfile[0], O_RDONLY);
    if (fd3 == -1) {
        perror("Errore nell'apertura del file per la lettura3");
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_read = read(fd3, buffer3, sizeof(buffer3) - 1);
    if (bytes_read == -1) {
        perror("Errore nella lettura del file");
        close(fd3);
        exit(EXIT_FAILURE);
    }
    buffer3[bytes_read] = '\0';
    printf("\nContenuto letto dal file3:\n%s", buffer3);

    if (close(fd3) == -1) {
        perror("Errore nella chiusura del file dopo la lettura");
        exit(EXIT_FAILURE);
    }


    int fd4 = open(arrayfile[1], O_RDONLY);
    if (fd4 == -1) {
        perror("Errore nell'apertura del file per la lettura");
        exit(EXIT_FAILURE);
    }

    bytes_read = read(fd4, buffer4, sizeof(buffer4) - 1);
    if (bytes_read == -1) {
        perror("Errore nella lettura del file");
        close(fd4);
        exit(EXIT_FAILURE);
    }
    buffer4[bytes_read] = '\0';
    printf("Contenuto letto dal file4:\n%s", buffer4);

    if (close(fd4) == -1) {
        perror("Errore nella chiusura del file dopo la lettura");
        exit(EXIT_FAILURE);
    }

    token = strtok(buffer3, "\n");
    i=0;
    char *array3[MAX_TOKEN];
    while (token != NULL && i < MAX_TOKEN) {
        array3[i] = token;
        i++;
        token = strtok(NULL, "\n"); 
    }
    int len3=i;

    token = strtok(buffer4, "\n");
    i=0;
    char *array4[MAX_TOKEN];
    while (token != NULL && i < MAX_TOKEN) {
        array4[i] = token;
        i++;
        token = strtok(NULL, "\n"); 
    }
    int len4=i;


    int contoken[lentoken];
    for(int i=0;i<lentoken;i++){
        contoken[i]=0;
    }

    for(int i=0;i<lentoken;i++){
        printf("\nciclo sul token %s\n",arraytoken[i]);
    
        //controllo il primo file
        for(int z=0;z<len3;z++){
            printf("controllo %s e %s\n",arraytoken[i],array3[z]);
            if(strcmp(arraytoken[i],array3[z])==0){
                contoken[i]++;
            }
        }

        //controllo il secondo file
        for(int z=0;z<len4;z++){
            printf("controllo %s e %s\n",arraytoken[i],array4[z]);
            if(strcmp(arraytoken[i],array4[z])==0){
                contoken[i]++;
            }
        }

    }

    for(int i=0;i<lentoken;i++){
        printf("il token %s ha %d occorrenze\n",arraytoken[i],contoken[i]);
    }

    return 0;
}
