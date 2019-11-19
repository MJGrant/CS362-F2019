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

// OLD cardMinion params: int currentPlayer, int choice1, int choice2, struct gameState *state, int handPos
// NEW minionRefactor params: int choice1, int choice2, struct gameState *state, int handPos

// choice1 = get +2 coins
// choice2 = discard hand, draw 4 new cards, force all players with 5+ cards to also discard and draw 4

void randomTestCard2() {

    int iteration = 0;

    int choice1PlayerGained2Coins = 0;
    int choice2OpponentDiscardedHand = 0;
    int choice2OpponentDidNotDiscard = 0;

    while (iteration < MAX_ITERATIONS) {
        // arrange
        struct gameState state;
        int k[10] = {1,2,3,4,5,6,7,8,9,10};
        int numPlayers = getRandomNumberOfPlayers();
        int currentPlayer = rand() % numPlayers; // random between 0 and numPlayers

        initializeGame(numPlayers, k, 2, &state);

        state.coins = rand() % 101; // starting coins between 0 and 100
        int choice = rand() % 2+1; // random between 0-1, shifted to 1-2

        // randomize each player's hand count and cards, discard count and cards
        int randomHandCount;
        for (int i = 1; i < numPlayers; i++) {
            randomHandCount = randomizeHand(&state, i);
            randomizeDiscard(&state, i);
        }

        // randomize the position of the tribute card in the player's hand
        int randomHandPos = insertCardIntoHandAtRandomPosition(&state, currentPlayer, randomHandCount, minion);
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
            printf("Minion Random Test [Option 1: Gain +2 coins]\nIteration #%d, # players: %d, hand count: %d, hand pos: %d, discard count: %d, current player: %d \n", iteration, numPlayers, randomHandCount, randomHandPos, discardCountBefore, currentPlayer);
            minionRefactor(1, 0, &state, randomHandPos);

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
            choice1PlayerGained2Coins++;
        } else if (choice == 2) {
            // act - choice1 is 0, take an estate (if one exists in the supply)
            printf("Minion Random Test [Option 2: Discard hand, draw 4, opponents do the same]\nIteration #%d, # players: %d, hand count: %d, hand pos: %d, discard count: %d, current player: %d \n", iteration, numPlayers, randomHandCount, randomHandPos, discardCountBefore, currentPlayer);
            minionRefactor(0, 1, &state, randomHandPos);

            // asserts

            // any and every player who used to have 5 or more cards in their hand
            // should have been forced to discard and draw 4 replaecment cards
            for (int i = 0; i < numPlayers; i++) {
                printf("- Player #%d: hand count before: %d, discard count before: %d\n", i, playerHandCountBefore[i], playerDiscardCountBefore[i]);
                if (playerHandCountBefore[i] >= 5) {
                    assertEqual("-- player was forced to discard hand and draw 4 cards", 4, state.handCount[i]);
                    // fails because cardMinion puts discarded cards in "played pile" instead of discard
                    assertEqual("-- player's discard pile grew by discarded amount", playerDiscardCountBefore[i]+playerHandCountBefore[i], state.discardCount[i]);
                    choice2OpponentDiscardedHand++;
                } else {
                    // opponent was not forced to redraw because opponent has 4 or fewer cards in hand
                    // verify hand size is the same
                    // but only do this check for opponents, not current player
                    // current player is not beholden to the "must have 5+ cards in hand to redraw" rule
                    if (i != currentPlayer) {
                        assertEqual("-- player's hand size remains unchanged", playerHandCountBefore[i], state.handCount[i]);
                        choice2OpponentDidNotDiscard++;
                    }
                }
            }
        }
    }



    if (iteration == MAX_ITERATIONS) {
        printf("Done with %d iterations\n", MAX_ITERATIONS);

        assertAtLeast("Tested scenario [player discarded Minion for +2 coins] at least 300 randomly-chosen ways", 300, choice1PlayerGained2Coins);
        assertAtLeast("Tested scenario [player forced another player to discard hand] at least 300 randomly-chosen ways", 300, choice2OpponentDiscardedHand);
        assertAtLeast("Tested scenario [player did not force another player to discard hand] at least 300 randomly-chosen ways", 300, choice2OpponentDidNotDiscard);
    }
}

int main() {
    srand(time(NULL));
    randomTestCard2();
    return 0;
}
