#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define is_number(c) ( (c >= 0x30 && c <= 0x39) )
#define ascii_to_int(c) ( c - 0x30 )
#define int_to_ascii(num) ( num + 0x30 )

#define fail(message) { perror(message); exit(EXIT_FAILURE); }
#define SPELLED_NUMBERS {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

void print_usage(char* exe_name);
int obtain_value(char* line, int size);
void look_for_spelled_numbers(char* line, int* first_ocurrence, int* last_ocurrence);
void transform_line(char* line, int* first_ocurrence, int* last_ocurrence);

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
        printf("Line: %s; Calibration value = %d\n\n", line, calibration_value);
        total_sum += calibration_value;
    }

    printf("\n\nTotal sum = %d\n", total_sum);

    exit(EXIT_SUCCESS);
}


void print_usage(char* exe_name) {
    printf("Usage:\n  %s <calibration_document>\n\n", exe_name);
}


int obtain_value(char* line, int size) {
    int first = -1, last = -1;  // Not valid values
    int i;
    int first_ocurrence[9], last_ocurrence[9];

    look_for_spelled_numbers(line, first_ocurrence, last_ocurrence);
    printf("Original line:    %s\n", line);
    transform_line(line, first_ocurrence, last_ocurrence);
    printf("Trasnformed line: %s\n", line);
    
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

void look_for_spelled_numbers(char* line, int* first_ocurrence, int* last_ocurrence) {
    int i;
    char* spelled_numbers[] = SPELLED_NUMBERS;
    char* ocurrence;
    int first, last;

    for (i = 0; i < 9; i++) {
        first = -1;
        ocurrence = line;
        do {
            ocurrence = strstr(ocurrence, spelled_numbers[i]);
            if (ocurrence != NULL) {
                if (first == -1) {
                    first = ocurrence - line;
                }
                last = ocurrence - line;
            }
            ocurrence++;
        } while (ocurrence != (char*) 1);
        if (first == -1) {
            first_ocurrence[i] = -1;
            last_ocurrence[i] = -1;
        }
        else {
            first_ocurrence[i] = first;
            last_ocurrence[i] = last;
        }
    }
}

void transform_line(char* line, int* first_ocurrence, int* last_ocurrence) {
    int i;
    int first, last;

    for (i = 0; i < 9; i++) {
        first = first_ocurrence[i];
        last = last_ocurrence[i];
        if (first != -1) {
            line[first] = int_to_ascii(i + 1);
            line[last] = int_to_ascii(i + 1);
        }
    }
}
