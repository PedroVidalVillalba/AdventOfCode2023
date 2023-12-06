#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define fail(message) { perror(message); exit(EXIT_FAILURE); }

#define is_number(c) ( c >= 0x30 && c <= 0x39 )

#define SIZE 1024

typedef struct {
    uint32_t number;
    bool winning_numbers[100];
    uint32_t* card_numbers;
    size_t amount_of_numbers;
} Card;

typedef struct {
    size_t size;
    Card* cards;
    uint32_t* amounts;
} Deck;

Card create_card(char* card_line);
void delete_card(Card* card);

Deck create_deck(Card* cards, size_t size);
void delete_deck(Deck* deck);

void get_winning_numbers(bool* winning_numbers, char* winning_buffer);
size_t get_card_numbers(bool* winning_numbers, char* card_buffer, uint32_t** card_numbers);

uint32_t count_matches(Card card);
uint32_t play_game(Deck deck);

int main(int argc, char** argv) {
    FILE* input;
    char line[SIZE];
    Card* cards;
    Deck deck;
    bool first;
    struct stat statbuf;
    int i;
    size_t size;
    uint32_t score;

    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ( (input = fopen(argv[1], "r")) == NULL ) fail("Couldn't open input file");
    stat(argv[1], &statbuf);

    first = true;
    i = 0;
    while (fgets(line, SIZE, input) != NULL) {
        if (first) {
            size = statbuf.st_size / strlen(line);
            cards = (Card *) malloc(size * sizeof(Card));
            first = false;
        }
        cards[i++] = create_card(line);
    }

    deck = create_deck(cards, size);

    score = play_game(deck);

    printf("Total number of scratchcards: %u\n", score);

    delete_deck(&deck);
    fclose(input);

    exit(EXIT_SUCCESS);
}

Card create_card(char* card_line) {
    Card card;
    char winning_buffer[SIZE] = {0}, card_buffer[SIZE] = {0};

    sscanf(card_line, " Card %u: %[^|] | %[^\r\n]", &card.number, winning_buffer, card_buffer);
    get_winning_numbers(card.winning_numbers, winning_buffer);
    card.amount_of_numbers = get_card_numbers(card.winning_numbers, card_buffer, &(card.card_numbers));

    return card;
}

void delete_card(Card* card) {
    card->number = 0;
    memset(card->winning_numbers, false, 100 * sizeof(bool));
    free(card->card_numbers);
    card->card_numbers = NULL;
}

Deck create_deck(Card* cards, size_t size) {
    Deck deck;
    int i;

    deck.size = size;

    deck.cards = cards;
    deck.amounts = (uint32_t *) malloc(size * sizeof(uint32_t));

    for (i = 0; i < size; i++) {
        deck.amounts[i] = 1;   // Start with one of each card
    }

    return deck;
}

void delete_deck(Deck* deck) {
    int i;

    free(deck->amounts);
    deck->amounts = NULL;
    for (i = 0; i < deck->size; i++) {
        delete_card(deck->cards + i);
    }
    free(deck->cards);
    deck->cards = NULL;
    deck->size = 0;
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

size_t get_card_numbers(bool* winning_numbers, char* card_buffer, uint32_t** card_numbers) {
    uint32_t number;
    uint32_t buffer[100];
    size_t i, j;

    i = 0; j = 0;
    while (card_buffer[i]) {
        number = 0;
        while (is_number(card_buffer[i])) {
            number *= 10;
            number += card_buffer[i] - 0x30;
            i++;
        }
        if (number > 0 && number <= 100 && winning_numbers[number - 1]) {
            buffer[j++] = number;
        }
        i++;
    }

    *card_numbers = (uint32_t *) malloc(j * sizeof(uint32_t));
    memcpy(*card_numbers, buffer, j * sizeof(uint32_t));

    return j;
}


uint32_t count_matches(Card card) {
    int i;
    uint32_t count;

    for (i = 0, count = 0; i < card.amount_of_numbers; i++) {
        if (card.winning_numbers[card.card_numbers[i] - 1]) count++;
    }
    
    return count;
}

uint32_t play_game(Deck deck) {
    uint32_t score;
    uint32_t matches;
    int i, j;

    score = 0;
    for (i = 0; i < deck.size; i++) {
        matches = count_matches(deck.cards[i]);
        for (j = 1; j <= matches && i + j < deck.size; j++) {
            deck.amounts[i + j] += deck.amounts[i];
        }
        score += deck.amounts[i];
    }

    return score;
}

