#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>


#define fail(message) { perror(message); exit(EXIT_FAILURE); }

#define SIZE 1024

typedef enum {
    part_number,
    unused_number,
    symbol,
    empty
} SymbolType;


char** process_schematic(char* schematic_file, uint32_t* rows, uint32_t* columns);

SymbolType encode(char c);
SymbolType** encode_matrix(char** engine_matrix, uint32_t rows, uint32_t columns);

void get_neighbours(uint32_t row, uint32_t col, SymbolType** encoded_matrix, uint32_t rows, uint32_t columns, SymbolType* neighbours);
void mark_part_numbers(SymbolType** encoded_matrix, uint32_t rows, uint32_t columns);

int main(int argc, char** argv) {
    char** engine_matrix;
    SymbolType** encoded_matrix;
    uint32_t columns, rows;
    int i, j, k, neighbour_row;
    int i1, j1;
    uint64_t part, gear_ratio, total_ratio;
    int neighbouring_part_numbers;
    SymbolType neighbours[9];

    if (argc != 2) {
        printf("Usage: %s <engine_schematic>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    engine_matrix = process_schematic(argv[1], &rows, &columns);
    encoded_matrix = encode_matrix(engine_matrix, rows, columns);

    mark_part_numbers(encoded_matrix, rows, columns);

    total_ratio = 0;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < columns; j++) {
            if (engine_matrix[i][j] == '*') {
                get_neighbours(i, j, encoded_matrix, rows, columns, neighbours);
                neighbouring_part_numbers = 0;
                for (k = 0; k < 9; k++) {   // Check for exactly two neighbouring part numbers 
                    if (neighbours[k] == part_number) {
                        neighbouring_part_numbers++;
                        neighbour_row = k / 3;
                        while ((k + 1) / 3 == neighbour_row && neighbours[k + 1] == part_number) k++;
                    }
                }
                if (neighbouring_part_numbers == 2) {   // Search part numbers and multiply them
                    gear_ratio = 1;
                    for (i1 = i - 1; i1 <= i + 1; i1++) {
                        for (j1 = j - 1; j1 <= j + 1; j1++) {
                            if (i1 < 0 || i1 >= rows || j1 < 0 || j1 >= columns || encoded_matrix[i1][j1] != part_number) continue;
                            // Go to the start of the number
                            while (j1 - 1 >= 0 && encoded_matrix[i1][j1 - 1] == part_number) j1--;
                            part = 0;
                            while (j1 < columns && encoded_matrix[i1][j1] == part_number) {
                                part *= 10;
                                part += engine_matrix[i1][j1] - 0x30;
                                j1++;
                            }
                            gear_ratio *= part;
                        }
                    }
                    total_ratio += gear_ratio;
                }
            }
        }
    }

    printf("Total ratio: %lu\n", total_ratio);

    for (i = 0; i < rows; i++) {
        free(engine_matrix[i]);
        free(encoded_matrix[i]);
    }
    free(engine_matrix);
    free(encoded_matrix);

    exit(EXIT_SUCCESS);
}


char** process_schematic(char* schematic_file, uint32_t* rows, uint32_t* columns) {
    FILE* schematic;
    char** engine_matrix;
    char buffer[SIZE];
    struct stat statbuf;
    int i;

    if ( (schematic = fopen(schematic_file, "r")) == NULL ) {
        fail("Couldn't open engine schematic");
    }

    /* Count the number of lines and columns for the engine matrix */
    /* The engine matrix is always square */

    /* Get line */
    fgets(buffer, SIZE, schematic);
    *columns = strlen(buffer) - 1;   // We'll remove the \n

    if (stat(schematic_file, &statbuf)) fail("Couldn't get stats for the file");

    *rows = statbuf.st_size / (*columns + 1);

    engine_matrix = (char **) calloc(*rows, sizeof(char *)); // Allocate rows
    rewind(schematic);
    for (i = 0; i < *rows; i++) {
        engine_matrix[i] = (char *) calloc(*columns + 1, sizeof(char));  // Make space for \0
        fscanf(schematic, " %[^\r\n]", buffer);
        memcpy(engine_matrix[i], buffer, *columns * sizeof(char));
    }

    fclose(schematic);

    return engine_matrix;
}


SymbolType** encode_matrix(char** engine_matrix, uint32_t rows, uint32_t columns) {
    SymbolType** encoded_matrix;
    int i, j;

    encoded_matrix = (SymbolType **) malloc(rows * sizeof(SymbolType *));
    for (i = 0; i < rows; i++) {
        encoded_matrix[i] = (SymbolType *) malloc(columns * sizeof(SymbolType));
        for (j = 0; j < columns; j++) {
            encoded_matrix[i][j] = encode(engine_matrix[i][j]);
        }
    } 

    return encoded_matrix;
}

SymbolType encode(char c) {
    switch (c) {
        case '0' ... '9': 
            return unused_number;

        case '.':
            return empty;

        default:
            return symbol;
    }
}

void mark_part_numbers(SymbolType** encoded_matrix, uint32_t rows, uint32_t columns) {
    SymbolType neighbours[9];
    int i, j, k, l;

    for (i = 0; i < rows; i++) {
        for (j = 0; j < columns; j++) {
            if (encoded_matrix[i][j] == unused_number) {
                get_neighbours(i, j, encoded_matrix, rows, columns, neighbours);
                for (k = 0; k < 9; k++) {   // Check for symbols in the neighbours
                    if (neighbours[k] == symbol) {
                        encoded_matrix[i][j] = part_number;
                        /* Propagate to neighbouring unused numbers */
                        l = -1; // Propagate left
                        while (j + l >= 0 && encoded_matrix[i][j + l] == unused_number) {
                            encoded_matrix[i][j + l] = part_number;
                            l--;
                        }
                        l = 1;  // Propagate right
                        while (j + l < rows && encoded_matrix[i][j + l] == unused_number) {
                            encoded_matrix[i][j + l] = part_number;
                            l++;
                        }
                        break;
                    }
                }
            }
        }
    }

}


void get_neighbours(uint32_t row, uint32_t col, SymbolType** encoded_matrix, uint32_t rows, uint32_t columns, SymbolType* neighbours) {
    int i, j;

    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if (row + i < 0 || row + i >= rows || col + j < 0 || col + j >= columns) {  // Cell is in the edge
                neighbours[3 * (i + 1) + (j + 1)] = empty;
            } else {
                neighbours[3 * (i + 1) + (j + 1)] = encoded_matrix[row + i][col + j];
            }
        }
    }

}
