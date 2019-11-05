#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "my_utils.h"
#include "dominion.h"
#include <time.h>
#include <string.h>

// ************************
// Random tests for Minion card

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state

// cardMinion params: int currentPlayer, int choice1, int choice2, struct gameState *state, int handPos
// choice1 = get +2 coins
// choice2 = discard hand, draw 4 new cards, force all players with 5+ cards to also discard and draw 4

void randomTestCard2() {

    int iteration = 0;

    while(1) {
        // arrange
        struct gameState state;
        int k[10] = {1,2,3,4,5,6,7,8,9,10};
        int currentPlayer = 1;
        int numPlayers = rand() % 2 + 2; // random between 0-2, then shift to 2-4

        initializeGame(numPlayers, k, 2, &state);

        state.coins = rand() % 101; // starting coins between 0 and 100
        int choice = rand() % 2+1; // random between 0-1, shifted to 1-2

        int randomHandCount;
        int randomDiscardCount;

        for (int i = 1; i < numPlayers; i++) {
            // randomize hand size cards for each player
            randomHandCount = getRandomHandCount();
            state.handCount[i] = randomHandCount;
            setRandomHand(&state, i, randomHandCount);

            // randomize discard pile size and cards
            randomDiscardCount = getRandomDiscardCount();
            state.discardCount[i] = randomDiscardCount;
            setRandomDiscardPile(&state, i, randomDiscardCount);
        }

        // randomize the position of the minion in the player's hand
        int randomHandPos = 0;
        if (randomHandCount > 0) {
            randomHandPos = rand() % randomHandCount;
            state.hand[currentPlayer][randomHandPos] = minion;
        }

        int discardCountBefore = state.discardCount[currentPlayer];
        int coinsBefore = state.coins;

        // record player hand sizes
        int playerHandCountBefore[4] = {0,0,0,0};
        int playerDiscardCountBefore[4] = {0,0,0,0};
        for (int i = 0; i < numPlayers; i++) {
            playerHandCountBefore[i] = state.handCount[i];
            playerDiscardCountBefore[i] = state.discardCount[i];
        }

        iteration++;

        if (choice == 1) {
            // act
            printf("Minion Random Test [Option 1: Gain +2 coins]\nIteration #%d, # players: %d, hand count: %d, hand pos: %d, discard count: %d \n", iteration, numPlayers, randomHandCount, randomHandPos, discardCountBefore);
            cardMinion(currentPlayer, 1, 0, &state, randomHandPos);

            //assert
            assertEqual("Player gained 2 coins", coinsBefore+2, state.coins);
            for (int i = 0; i < numPlayers; i++) {
                printf("- Player #%d: hand count before: %d, discard count before: %d\n", i, playerHandCountBefore[i], playerDiscardCountBefore[i]);
                if (i == currentPlayer) {
                    assertDecreasedByOne("-- player discarded minion from hand", playerHandCountBefore[i], state.handCount[i]);
                } else {
                    assertEqual("-- player's hand count remains unchanged", playerHandCountBefore[i], state.handCount[i]);
                }
                assertEqual("-- player's discard pile remains unchanged", playerDiscardCountBefore[i], state.discardCount[i]);
            }
        } else if (choice == 2) {
            // act - choice1 is 0, take an estate (if one exists in the supply)
            printf("Minion Random Test [Option 2: Discard hand, draw 4, opponents do the same]\nIteration #%d, # players: %d, hand count: %d, hand pos: %d, discard count: %d, \n", iteration, numPlayers, randomHandCount, randomHandPos, discardCountBefore);
            cardMinion(currentPlayer, 0, 1, &state, randomHandPos);

            // asserts

            // any and every player who used to have 5 or more cards in their hand
            // should have been forced to discard and draw 4 replaecment cards
            for (int i = 0; i < numPlayers; i++) {
                printf("- Player #%d: hand count before: %d, discard count before: %d\n", i, playerHandCountBefore[i], playerDiscardCountBefore[i]);
                if (playerHandCountBefore[i] >= 5) {
                    assertEqual("-- player was forced to discard hand and draw 4 cards", 4, state.handCount[i]);
                    // fails because cardMinion puts discarded cards in "played pile" instead of discard
                    assertEqual("-- player's discard pile grew by discarded amount", playerDiscardCountBefore[i]+playerHandCountBefore[i], state.discardCount[i]);
                }
            }

        }

        if (iteration == MAX_ITERATIONS) {
            printf("Done with %d iterations\n", MAX_ITERATIONS);
            break;
        }
    }
}

int main() {
    srand(time(NULL));
    randomTestCard2();
    return 0;
}
