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

int calculate_power(char* game);

int main(int argc, char** argv) {
    FILE* input;
    char game[SIZE];
    int power, power_sum;

    if (argc != 2) {
        printf("Usage: %s <input>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ( (input = fopen(argv[1], "r")) == NULL) {
        fail("Couldn't open input file");
    }

    power_sum = 0;
    while (fscanf(input, " %[^\r\n]", game) != EOF) {
        power = calculate_power(game);
        printf("%s: Power = %d\n", game, power);
        power_sum += power;
    }

    printf("Total power sum = %d\n", power_sum);

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


int calculate_power(char* game) {
    char* buffer;
    int current_set[3], minimal_set[3];
    int count;
    int power;
    int i;
    Color color;
    char color_word[32];

    /* Place a ; at the end for strtok */
    game[strlen(game) + 1] = '\0';
    game[strlen(game)] = ';';

    /* Identify the game and set the string in strtok */
    buffer = strtok(game, ":");
    
    memset(minimal_set, 0, 3 * sizeof(int));
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
            if (current_set[i] > minimal_set[i]) {
                minimal_set[i] = current_set[i];
            }
        }
    }

    power = 1;
    for (i = 0; i < 3; i++) {
        power *= minimal_set[i];
    }

    return power;
}

