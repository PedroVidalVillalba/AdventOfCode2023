#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <stdint.h>

#define fail(message) { perror(message); exit(EXIT_FAILURE); }

#define NUM_MAPS 7

struct node {
    uint64_t destination_range_start;
    uint64_t source_range_start;
    uint64_t range_length;
    struct node* next;
};

typedef struct node* List;


typedef struct {
    char* name;
    List list;
} Map;


void create_map(Map* map, char* start, char* len);
void delete_map(Map* map);


uint64_t map_value(uint64_t value, Map map);


int main(int argc, char** argv) {
    int input_fd;
    char* input;
    struct stat statbuf;
    char* start;
    char* end;
    uint64_t seeds[128];
    int i, j;
    Map maps[NUM_MAPS];
    uint64_t min_value, value;

    if (argc != 2) {
        printf("Usage: %s <input>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ( (input_fd = open(argv[1], O_RDONLY)) == -1) fail("Couldn't open input file");

    if (fstat(input_fd, &statbuf) == -1) fail("Couldn't obtain stats for the input file");

    if ( (input = (char*) mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, input_fd, 0)) == MAP_FAILED) fail("Couldn't map file into memory");

    /* Move to the start of the seeds */
    start = strchr(input, ' ');
    end = strstr(start, "\n\n");
    memset(seeds, -1, 128 * sizeof(uint64_t));
    i = 0;
    while (start < end) {
        sscanf(start++, " %lu", &seeds[i]); // Also advance to move from the space
        start = strchr(start, ' ');
        i++;
    }

    /* Create maps */
    for (j = 0; j < NUM_MAPS; j++) {
        end += 2;  // Advance to next block
        start = end;
        end = strstr(start, "\n\n");
        create_map(maps + j, start, end);
    }

    min_value = (uint64_t) -1;
    i = 0;
    while (seeds[i] != -1) {
        value = seeds[i];
        for (j = 0; j < NUM_MAPS; j++) {
            value = map_value(value, maps[j]);
        }
        min_value = (value < min_value ? value : min_value);
        i++;
    }

    printf("Lowest location number: %lu\n", min_value);

    close(input_fd);
    munmap(input, statbuf.st_size);
    for (j = 0; j < NUM_MAPS; j++) {
        delete_map(maps + j);
    }
   
    exit(EXIT_SUCCESS);
}


void create_map(Map* map, char* start, char* end) {
    List next, current;
    bool first_iter;


    sscanf(start, " %ms", &(map->name));

    start = strchr(start, '\n') + 1;
    first_iter = true;
    map->list = NULL;
    while(start < end) {
        next = (List) malloc(sizeof(struct node));
        sscanf(start, "%lu %lu %lu", &(next->destination_range_start), &(next->source_range_start), &(next->range_length));

        next->next = NULL;

        if (first_iter) {
            map->list = next;
            current = next;
            first_iter = false;
        } else {
            current->next = next;
            current = next;
        }

        /* Go to the next line */
        start = strchr(start, '\n') + 1;
    }
}

void delete_map(Map* map) {
    List next;

    free(map->name);
    map->name = NULL;

    while (map->list) {
        next = map->list->next;
        free(map->list);
        map->list = next;
    }
}


uint64_t map_value(uint64_t value, Map map) {
    uint64_t result;
    
    while (map.list) {
        if (value >= map.list->source_range_start && value < map.list->source_range_start + map.list->range_length) {
            result = map.list->destination_range_start + (value - map.list->source_range_start);
            break;
        }
        map.list = map.list->next;
    }

    if (!map.list) result = value; // List finished without matches

    return result;
}
