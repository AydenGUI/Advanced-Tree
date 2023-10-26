#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int testTree(int num, char *prefix, char *word) {

    if (num <= 0)
        return 0;
    char newWord[strlen(word) + strlen(prefix)];
    strncpy(newWord, prefix, strlen(prefix) - 2);
    strcat(newWord, "\0");
    strcat(newWord, word);
    printf("Num %d: %s\n", num, newWord);
    testTree(num - 1, prefix, newWord);//, word);
    return 0;
}
int main () {

    testTree(3, "lolpop", "joint");
}
