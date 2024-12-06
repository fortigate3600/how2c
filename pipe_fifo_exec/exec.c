#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

int esempio() {
    pid_t pid = fork();

    if (pid < 0) {
        // Error in fork
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        printf("Child process (PID: %d) executing 'ls' command\n", getpid());
        execl("/bin/ls", "ls", "-l", NULL);
        // execl only returns if it fails
        perror("exec failed");
        exit(1);
    } else {
        // Parent process
        printf("Parent process (PID: %d) waiting for child to complete\n", getpid());
        wait(NULL);
        printf("Child process finished\n");
    }

    return 0;
}

//un argomento
int main1(int argc, char* argv[]){
    if(argc<=2){
        printf("no valid argument in input\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        // Error in fork
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        char* command = malloc(5+strlen(argv[1]));
        strcat(command, "/bin/");
        strcat(command, argv[1]);

        printf("Child process (PID: %d) executing '%s' command\n", getpid(), argv[1]);
        execl(command, command, argv[2], NULL);
        
        // execl only returns if it fails
        perror("exec failed");
        exit(1);
    } else {
        // Parent process
        printf("Parent process (PID: %d) waiting for child to complete\n", getpid());
        wait(NULL);
        printf("Child process finished\n");
    }

    return 0;
}

// n argomenti
int main2(int argc, char* argv[]){
    if(argc<=1){
        printf("no valid argument in input\n");
        exit(EXIT_FAILURE);
    }
    char** arguments = malloc((argc+1)*sizeof(char*)); // quel +1 è per il NULL finale
    arguments=argv;
    arguments++;
    for(int i=0;i<argc;i++){
        printf("%s ",arguments[i]);
    }
    printf("\n");

    pid_t pid = fork();

    if (pid < 0) {
        // Error in fork
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process

        printf("Child process (PID: %d) executing '%s' command\n", getpid(), arguments[0]);
        execvp(arguments[0], arguments);
        
        // execl only returns if it fails
        perror("exec failed");
        exit(1);
    } else {
        // Parent process
        printf("Parent process (PID: %d) waiting for child to complete\n", getpid());
        wait(NULL);
        printf("Child process finished\n");
    }

    return 0;
}

//mini workig shell
int main3(){
    char** arguments;
    while(1){

        printf("shell: ");
        char input[100];
        fgets(input, sizeof(input), stdin);

        int lenarg=0;
        arguments=NULL;
        char* token=strtok(input," \n\0");
        while (token != NULL) {
            arguments = realloc(arguments, (lenarg + 1) * sizeof(char*));
            arguments[lenarg++] = token;
            token = strtok(NULL, " \n\0");
        }
        arguments = realloc(arguments, (lenarg + 1) * sizeof(char*));
        arguments[lenarg] = NULL;

        for(int i=0;i<lenarg;i++){
            printf("%s ",arguments[i]);
        }
        printf("\n");

        pid_t pid = fork();

        if (pid < 0) {
            // Error in fork
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            // Child process

            printf("Child process (PID: %d) executing '%s' command\n", getpid(), arguments[0]);
            execvp(arguments[0], arguments);
            
            // execl only returns if it fails
            perror("exec failed");
            exit(1);
        }
        // Parent process
        printf("Parent process (PID: %d) waiting for child to complete\n", getpid());
        wait(NULL);
        printf("Child process finished\n");
    }
    free(arguments);
    return 0;
}

// implemtento il reindiriazzamento dell'output ex. ls > output.txt
int main(){
    char** arguments;
    while(1){

        printf("shell: ");
        char input[100];
        fgets(input, sizeof(input), stdin);

        int lenarg=0;
        arguments=NULL;
        char* token=strtok(input," \n\0");
        while (token != NULL) {
            arguments = realloc(arguments, (lenarg + 1) * sizeof(char*));
            arguments[lenarg++] = token;
            token = strtok(NULL, " \n\0");
        }
        arguments = realloc(arguments, (lenarg + 1) * sizeof(char*));
        arguments[lenarg] = NULL;

        // Verifica se c'è il simbolo ">" per la redirezione
        int redirect_index = -1;
        char* output_file = NULL;
        for (int i = 0; i < lenarg; i++) {
            if (strcmp(arguments[i], ">") == 0 && i + 1 < lenarg) {
                redirect_index = i;
                output_file = arguments[i + 1]; // Nome del file di output
                break;
            }
        }

        if (redirect_index != -1) {
            // Tronca gli argomenti al simbolo ">"
            arguments[redirect_index] = NULL;
        }

        for(int i=0;i<lenarg;i++){
            printf("%s ",arguments[i]);
        }
        printf("\n");

        pid_t pid = fork();

        if (pid < 0) {
            // Error in fork
            perror("fork failed");
            exit(1);
        } else if (pid == 0) {
            // Child process
            if (output_file != NULL) {
                // Apre il file in modalità scrittura
                int fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd < 0) {
                    perror("open failed");
                    exit(1);
                }
                // Redirige stdout verso il file
                if (dup2(fd, STDOUT_FILENO) < 0) {
                    perror("dup2 failed");
                    exit(1);
                }
                close(fd); // Chiude il file descriptor originale
            }

            printf("Child process (PID: %d) executing '%s' command\n", getpid(), arguments[0]);
            execvp(arguments[0], arguments);
            
            // execl only returns if it fails
            perror("exec failed");
            exit(1);
        }
        // Parent process
        printf("Parent process (PID: %d) waiting for child to complete\n", getpid());
        wait(NULL);
        printf("Child process finished\n");
    }
    free(arguments);
    return 0;
}
