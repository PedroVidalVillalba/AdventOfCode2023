#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define fail(message) { perror(message); exit(EXIT_FAILURE); }

#define MAX_RACES 64

int read_input(char* input_file, uint64_t* times, uint64_t* distances);

uint64_t count_ways(uint64_t time, uint64_t record);

int main(int argc, char** argv) {
    uint64_t times[MAX_RACES] = {0};
    uint64_t distances[MAX_RACES] = {0};
    int num_races;
    int i;
    uint64_t total, ways;

    if (argc != 2) {
        printf("Usage: %s <input>\n", argv[0]);
    }

    num_races = read_input(argv[1], times, distances);

    total = 1;
    for (i = 0; i < num_races; i++) {
        ways = count_ways(times[i], distances[i]);
        printf("Race %d; Ways of beating de record: %lu\n", i+1, ways);
        total *= ways;
    }

    printf("Total ways of beating all the records: %lu\n", total);

    exit(EXIT_SUCCESS);
}

int read_input(char* input_file, uint64_t* times, uint64_t* distances) {
    FILE* input;
    int num_races;
    int i;

    if ( (input = fopen(input_file, "r")) == NULL ) fail("Couldn't open input file");

    fscanf(input, "Time:");
    i = 0;
    while (fscanf(input, " %lu", times + i) > 0) i++;
    fscanf(input, "Distance:");
    i = 0;
    while (fscanf(input, " %lu", distances + i) > 0) i++;
    num_races = i;

    fclose(input);

    return num_races;
}


uint64_t count_ways(uint64_t time, uint64_t record) {
    uint64_t i;
    uint64_t ways;

    ways = 0;
    for (i = 1; i < time; i++) {
        if (i * (time - i) > record) {
            ways++;
        }
    }

    return ways;
}
