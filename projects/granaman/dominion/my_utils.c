// my_utils methods

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "my_utils.h"
#include "dominion.h"

int getRandomHandCount() {
    int random = rand() % MAX_HAND_SIZE; // between 0 and max hand size
    return random;
}

int getRandomDiscardCount() {
    int random = rand() % MAX_DECK; // between 0 and 500 as defined in dominion.h
    return random;
}

int getRandomCard() {
    int card = rand() % 27; // between 0 and 26
    return card;
}

void setRandomHand(struct gameState *state, int currentPlayer, int randomHandCount) {
    for (int i = 0; i < randomHandCount; i++) {
        state->hand[currentPlayer][i] = getRandomCard();
    }
}

void setRandomDiscardPile(struct gameState *state, int currentPlayer, int randomDiscardCount) {
    for (int i = 0; i < randomDiscardCount; i++) {
        state->discard[currentPlayer][i] = getRandomCard();
    }
}

void setRandomEstateQuantity(struct gameState *state) {
    state->supplyCount[estate] = rand() % 9; //random number of estates between 0 and 8
}
