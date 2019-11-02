#include <stdlib.h>
#include <stdio.h>
#include "dominion.h"
#include <time.h>

// ************************
// Random tests for Baron card

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state

// cardBaron params: numPlayer, choice1, state
// choice1 is 1 = discard an estate or gain an estate
// choice1 is 0 = gain an estate (if there are any available)

int MAX_HAND_SIZE = 10; // up to 500 is allowed but realistically the player's hand size is rarely over 10

int getRandomHandCount() {
    int random = rand() % MAX_HAND_SIZE + 1;
    return random;
}

int getRandomCard() {
    // 0 - 26 possible cards
    int card = rand() % 27;
    return card;
}


void setRandomHand(struct gameState *state, int currentPlayer, int randomHandCount) {
    for (int i = 0; i < randomHandCount; i++) {
        state->hand[currentPlayer][i] = getRandomCard;
    }
}

void setRandomEstateQuantity(struct gameState *state) {
    state->supplyCount[estate] = rand() % 9; //random number of estates between 0 and 8
}

void randomTestCard1() {

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);

    int iteration = 0;

    // ********************************
    // Choice 1: The player wants to gain an estate
        // randomize the size of the player's hand
        // randomize the cards in the player's hand
        // randomize the quantity of estates in the estate pile
    while(1) {
        iteration++;
        int randomHandCount = getRandomHandCount();

        state.handCount[currentPlayer] = randomHandCount;
        setRandomHand(&state, currentPlayer, randomHandCount);
        setRandomEstateQuantity(&state);

        printf("Baron Random Test - Iteration %d, hand count: %d, estate supply pile: %d \n", iteration, randomHandCount, state.supplyCount[estate]);

        // act
        // passing a zero as the second param indicates user wants to gain an estate
        int ret = cardBaron(currentPlayer, 0, &state);

        if (ret == -1) {
            printf("Player unable to gain an estate\n");
            exit(200);
        } else if (iteration == 1000) {
            printf("Done with 1000 iterations\n");
            exit(200);
        }

    }
}

int main() {
    srand(time(NULL));
    randomTestCard1();
    return 0;
}
