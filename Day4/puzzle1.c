#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define fail(message) { perror(message); exit(EXIT_FAILURE); }

#define is_number(c) ( c >= 0x30 && c <= 0x39 )

#define SIZE 1024


void get_winning_numbers(bool* winning_numbers, char* winning_buffer);
uint64_t get_card_points(bool* winning_numbers, char* card_buffer);

int main(int argc, char** argv) {
    FILE* input;
    char winning_buffer[SIZE], card_buffer[SIZE];
    bool winning_numbers[100];
    uint32_t card_number;
    uint64_t card_points, total_points;

    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ( (input = fopen(argv[1], "r")) == NULL ) fail("Couldn't open input file");

    total_points = 0;
    while (fscanf(input, " Card %u: %[^|] | %[^\r\n]", &card_number, winning_buffer, card_buffer) != EOF) {
        get_winning_numbers(winning_numbers, winning_buffer);
        card_points = get_card_points(winning_numbers, card_buffer);
        printf("Game %d; Card points: %lu\n", card_number, card_points);
        total_points += card_points;
    }

    printf("Total points: %lu\n", total_points);

    exit(EXIT_SUCCESS);
}

void get_winning_numbers(bool* winning_numbers, char* winning_buffer) {
    uint32_t number;
    int i;

    memset(winning_numbers, false, 100 * sizeof(bool));
    i = 0;
    while (winning_buffer[i]) {
        number = 0;
        while (is_number(winning_buffer[i])) {
            number *= 10;
            number += winning_buffer[i] - 0x30;
            i++;
        }
        if (number > 0 && number <= 100) winning_numbers[number - 1] = true;
        i++;
    }
}

uint64_t get_card_points(bool* winning_numbers, char* card_buffer) {
    uint32_t number;
    uint64_t card_points;
    int i;

    card_points = 0;
    i = 0;
    while (card_buffer[i]) {
        number = 0;
        while (is_number(card_buffer[i])) {
            number *= 10;
            number += card_buffer[i] - 0x30;
            i++;
        }
        if (number > 0 && number <= 100 && winning_numbers[number - 1]) {
            if (!card_points) {
                card_points = 1;
            } else {
                card_points <<= 1;
            }
        }
        i++;
    }

    return card_points;
}
