#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define is_number(c) ( (c >= 0x30 && c <= 0x39) )
#define ascii_to_int(c) ( c - 0x30 )

#define fail(message) { perror(message); exit(EXIT_FAILURE); }

void print_usage(char* exe_name);
int obtain_value(char* line, int size);

int main(int argc, char** argv) {
    char* calibration_document;
    FILE* calibration_file;
    char line[1024] = {0};
    int total_sum;
    int calibration_value;


    if (argc != 2) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    calibration_document = argv[1];
    if ( (calibration_file = fopen(calibration_document, "r")) == NULL) {
        fail("Couldn't open calibration document");
    }

    total_sum = 0;
    while(fscanf(calibration_file, " %[^\r\n]", line) != EOF) {
        calibration_value = obtain_value(line, strlen(line));
        printf("Line: %s; Calibration value = %d\n", line, calibration_value);
        total_sum += calibration_value;
    }

    printf("\nTotal sum = %d\n", total_sum);

    exit(EXIT_SUCCESS);
}


void print_usage(char* exe_name) {
    printf("Usage:\n  %s <calibration_document>\n\n", exe_name);
}


int obtain_value(char* line, int size) {
    int first = -1, last = -1;  // Not valid values
    int i;

    for (i = 0; i < size; i++) {
        if (is_number(line[i]) && first == -1) {
            first = ascii_to_int(line[i]);
        }
        if (is_number(line[size - i - 1]) && last == -1) {
            last = ascii_to_int(line[size - i - 1]);
        }
    }

    if (first == -1 || last == -1) {    // Something went wrong
        return -1;
    }

    return 10 * first + last;
}
