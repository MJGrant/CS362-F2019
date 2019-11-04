#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "my_utils.h"
#include "dominion.h"
#include <time.h>

// ************************
// Random tests for Minion card

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state

// cardMinion params: int currentPlayer, int choice1, int choice2, struct gameState *state, int handPos
// choice1 = get +2 coins
// choice2 = discard hand, draw 4 new cards, force all players with 5+ cards to also discard and draw 4

void randomTestCard2() {

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);

    int iteration = 0;

    while(1) {
        iteration++;
        state.coins = rand()%101; // starting coins between 0 and 100
        int choice = rand()%2+1; // random between 0 and 1, offsetted +1 (so 1 and 2)

        // randomize hand size and cards
        int randomHandCount = getRandomHandCount();
        state.handCount[currentPlayer] = randomHandCount;
        setRandomHand(&state, currentPlayer, randomHandCount);

        // randomize the position of the minion in the player's hand
        int randomHandPos = 0;
        if (randomHandCount > 0) {
            randomHandPos = rand() % randomHandCount;
            state.hand[currentPlayer][randomHandPos] = minion;
        }

        // randomize discard pile size and cards
        int randomDiscardCount = getRandomDiscardCount();
        state.discardCount[currentPlayer] = randomDiscardCount;
        setRandomDiscardPile(&state, currentPlayer, randomDiscardCount);

        int discardCountBefore = state.discardCount[currentPlayer];
        int coinsBefore = state.coins;

        if (choice == 1) {
            // act
            printf("Minion Random Test [Option 1: Gain +2 coins]\nIteration #%d, hand count: %d, hand pos: %d, discard count: %d \n", iteration, randomHandCount, randomHandPos, discardCountBefore);
            cardMinion(currentPlayer, 1, 0, &state, randomHandPos);

            //assert
            assertEqual("Player gained 2 coins", coinsBefore+2, state.coins);
        } else if (choice == 2) {
            // assert
            printf("Minion Random Test [Option 2: Discard hand, draw 4, opponents do the same]\nIteration #%d, hand count: %d, hand pos: %d, discard count: %d, \n", iteration, randomHandCount, randomHandPos, discardCountBefore);
            cardMinion(currentPlayer, 0, 1, &state, randomHandPos);

        } else {
            printf("Error setting choice to 1 or 2\n");
        }
        // act


        // act - choice1 is 0, take an estate (if one exists in the supply)
        cardBaron(currentPlayer, 0, &state);

        if (iteration == MAX_ITERATIONS) {
            printf("Done with %d iterations\n", MAX_ITERATIONS);
            break;
        }
    }
}

int main() {
    randomTestCard2();
    return 0;
}
