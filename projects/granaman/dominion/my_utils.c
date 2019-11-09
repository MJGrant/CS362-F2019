// my_utils methods

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "my_utils.h"
#include "dominion.h"

// helper method for determining what type of card a given card is
int getCardType(int card) {
    int ret = -1;
    if (card == copper || card == silver || card == gold) {
        // treasure card
        ret = 1;
    } else if (card == estate || card == duchy || card == province || card == gardens) {
        // victory card
        ret = 2;
    } else if (card == adventurer || card == council_room || card == feast || card == mine || card == remodel || card == smithy || card == village || card == baron || card == minion || card == steward || card == tribute || card == ambassador || card == cutpurse || card == embargo || card == outpost || card == salvager || card == sea_hag || card == treasure_map) {
        // action card
        ret = 3;
    } else if (card == great_hall) {
        //combo action-victory card
        ret = 4;
    } else if (card == curse) {
        // curse card
        ret = 5;
    }
    return ret;
}

int getRandomHandCount() {
    int random = rand() % MAX_HAND_SIZE; // between 0 and max hand size
    return random;
}

int getRandomDeckCount() {
    int random = rand() % MAX_DECK_SIZE; // between 0 and max deck count
    return random;
}

int getRandomDiscardCount() {
    int random = rand() % MAX_DISCARD_SIZE; // between 0 and max discard count(look in utils.h)
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

void setRandomDeck(struct gameState *state, int currentPlayer, int randomDeckCount) {
    for (int i = 0; i < randomDeckCount; i++) {
        state->deck[currentPlayer][i] = getRandomCard();
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

int getRandomNumberOfPlayers() {
    return rand() % 3 + 2; // random between 0-2, then shift to 2-4
}

int insertCardIntoHandAtRandomPosition(struct gameState *state, int currentPlayer, int handCount, int card) {
    int randomHandPos = 0;
    if (handCount > 0) {
        randomHandPos = rand() % handCount;
        state->hand[currentPlayer][randomHandPos] = card;
    }
    return randomHandPos;
}

int randomizeHand(struct gameState *state, int player) {
    // randomize hand size and card
    // return quantity of cards in hand
    int randomHandCount = getRandomHandCount();
    state->handCount[player] = randomHandCount;
    setRandomHand(state, player, randomHandCount);
    return randomHandCount;
}

int randomizeDeck(struct gameState *state, int player) {
    // now pick a new random size for the deck
    int randomDeckCount = getRandomDeckCount();
    state->deckCount[player] = randomDeckCount;
    // and fill it with randomly picked cards
    setRandomDeck(state, player, randomDeckCount);
    return randomDeckCount;
}

int randomizeDiscard(struct gameState *state, int player) {
    // randomize discard pile size and cards
    // return quantity of cards in discard pile
    int randomDiscardCount = getRandomDiscardCount();
    state->discardCount[player] = randomDiscardCount;
    setRandomDiscardPile(state, player, randomDiscardCount);
    return randomDiscardCount;
}
