#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define fail(message) {perror(message); exit(EXIT_FAILURE);}

#define SIZE 1024

typedef enum {
    Red = 0,
    Green = 1,
    Blue = 2,
    Error = -1
} Color;


Color color_code(char* word);

/**
 * Returns de ID of the game if it is possible
 * using <max_cubes>. Else, return -1.
 */
int is_possible(char* game, int* max_cubes);

int main(int argc, char** argv) {
    FILE* input;
    char game[SIZE];
    int max_cubes[] = {12, 13, 14};
    int id, id_sum;

    if (argc != 2) {
        printf("Usage: %s <input>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ( (input = fopen(argv[1], "r")) == NULL) {
        fail("Couldn't open input file");
    }

    id_sum = 0;
    while (fscanf(input, " %[^\r\n]", game) != EOF) {
        id = is_possible(game, max_cubes);
        if (id != -1) {
            printf("%s is possible!\n", game);
            id_sum += id;
        };
    }

    printf("Total ID sum: %d\n", id_sum);

    fclose(input);

    exit(EXIT_SUCCESS);
}

Color color_code(char* word) {
    Color color;
    if (!strcmp(word, "red")) {
        color = Red;
    } else if (!strcmp(word, "green")) {
        color = Green;
    } else if (!strcmp(word, "blue")) {
        color = Blue;
    } else {
        color = Error;
    }

    return color;
}


int is_possible(char* game, int* max_cubes) {
    int id;
    char* buffer;
    int current_set[3];
    int count;
    int i;
    Color color;
    char color_word[32];

    /* Place a ; at the end for strtok */
    game[strlen(game) + 1] = '\0';
    game[strlen(game)] = ';';

    /* Identify the game and set the string in strtok */
    buffer = strtok(game, ":");
    sscanf(buffer, "Game %d", &id);
    
    /* Iterate through the sets of each game */
    while ( (buffer = strtok(NULL, ";")) != NULL) {
        memset(current_set, 0, 3 * sizeof(int));
        do {
            sscanf(buffer, " %d %[^,]", &count, color_word);
            color = color_code(color_word);
            if (color == Error) {
                fprintf(stderr, "Unknown color on %s\n", game);
                exit(EXIT_FAILURE);
            } 
            current_set[color] = count;
            /* Move buffer to the next comma */
            buffer = strstr(buffer, ",");
            if (buffer) buffer[0] = ' ';    // Replace comma with space
        } while(buffer);

        for (i = 0; i < 3; i++) {
            if (current_set[i] > max_cubes[i]) {
                id = -1;  // Impossible game
            }
        }
    }

    return id;
}

