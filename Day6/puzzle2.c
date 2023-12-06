#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>

#define fail(message) { perror(message); exit(EXIT_FAILURE); }

#define is_number(c) ( c >= 0x30 && c <= 0x39 )

void read_input(char* input_file, uint64_t* time, uint64_t* record);

uint64_t count_ways(uint64_t time, uint64_t record);

int main(int argc, char** argv) {
    uint64_t time;
    uint64_t record;
    uint64_t ways;

    if (argc != 2) {
        printf("Usage: %s <input>\n", argv[0]);
    }

    read_input(argv[1], &time, &record);

    ways = count_ways(time, record);

    printf("Total ways of beating the record: %lu\n", ways);

    exit(EXIT_SUCCESS);
}

void read_input(char* input_file, uint64_t* time, uint64_t* record) {
    FILE* input;
    char buffer[256] = {0};
    int i;

    if ( (input = fopen(input_file, "r")) == NULL ) fail("Couldn't open input file");

    fscanf(input, " Time: %[^\r\n]", buffer);
    i = 0;
    *time = 0;
    while (buffer[i]) {
        if (is_number(buffer[i])) {
            *time *= 10;
            *time += buffer[i] - 0x30;
        };
        i++;
    };

    fscanf(input, " Distance: %[^\r\n]", buffer);
    i = 0;
    *record = 0;
    while (buffer[i]) {
        if (is_number(buffer[i])) {
            *record *= 10;
            *record += buffer[i] - 0x30;
        };
        i++;
    };

    fclose(input);

    return;
}


uint64_t count_ways(uint64_t time, uint64_t record) {
    double min, max, root;
    uint64_t min_time, max_time;

    /* It's just a parabola, calculate the roots and take the difference.
     *  t: time of the race
     *  r: record distance
     *  x: time pressing the button == speed of the boat
     *  d: distance traveled
     *
     * d = x * (t - x)
     *
     * So d > r <=> d - r > 0, i.e.
     *
     * x * (t - x) - r > 0 <=>
     * -x^2 + tx - r > 0 
     *
     * Solving for x when the equality holds:
     *
     * x = (t -+ sqrt(t^2 - 4 * r)) / 2
     *
     * So x \in ( (t - sqrt(t^2 - 4 * r)) / 2, (t + sqrt(t^2 - 4 * r)) / 2 )
     * to beat the record
     */

    root = sqrt(time * time - 4 * record);
    min = (time - root) / 2.;
    max = (time + root) / 2.;

    min_time = (uint64_t) ceil(min);
    max_time = (uint64_t) floor(max);

    printf("Min time: %lu; Max time: %lu\n", min_time, max_time);

    return max_time - min_time + 1; // Count both extremes
}
