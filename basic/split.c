#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char** split(const char* buffer, const char* token, int * len) {
    char* buffer_copy = strdup(buffer);
    char** array = NULL;
    *len=0;

    char* temp = strtok(buffer_copy, token);
    while (temp != NULL) {
        array = realloc(array, (*len + 1) * sizeof(char*));
        array[*len] = strdup(temp);
        (*len)++;
        temp = strtok(NULL, token);
    }
    return array;
}

int main() {
    const char buffer[] = "Questo è un esempio di stringa da tokenizzare a a a a";
    const char token[] = " ";
    int len=0;
    char** tokens = split(buffer, token, &len);

    for (int i = 0; tokens[i] != NULL; i++) {
        printf("Token %d: %s\n", i + 1, tokens[i]);
    }
    printf("len: %d\n",len);


    return 0;
}
