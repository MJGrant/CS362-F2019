#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "dominion.h"
#include <time.h>

// ************************
// Unit tests for Minion card

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state

// cardMinion params: int currentPlayer, int choice1, int choice2, struct gameState *state, int handPos
// choices are mutually exclusive
// if choice1 is 1 = discard minion card and get two coins
// else if choice2 is 1 = discard hand, redraw 4, other players with 5+ cards discard hand and draw 4

void minionTest1a() {
    printTestName("Minion Card", "Player discards Minion and gains +2 coins");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);

    int discardCountBefore = state.discardCount[currentPlayer];
    int numActionsBefore = state.numActions;
    int coinsBefore = state.coins;

    // set the player's hand to a specific arrangement of cards
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = baron;
    state.hand[currentPlayer][1] = minion; // one estate in hand but shouldn't affect outcome
    state.hand[currentPlayer][2] = copper;
    state.hand[currentPlayer][3] = copper;
    int handCountBefore = state.handCount[currentPlayer];

    // act
    // set choice1 (2nd param) to 1 (true) to use the "get 2 coins" card option
    // last param is "handPos", the position of the minion card in the player's hand
    cardMinion(currentPlayer, 1, 0, &state, 1);

    // assert
    assertIncreasedByOne("The player gained an action", numActionsBefore, state.numActions);

    // verifying that the discard method was used properly
    // some of these FAIL because the discardCard method is full of problems
    // verify that the player's hand count decreased by one
    assertDecreasedByOne("The player's hand size has decreased by one", handCountBefore, state.handCount[currentPlayer]);
    // verify that the player's discard pile count went up by 1
    assertIncreasedByOne("[KNOWN BUG] Player's discard pile count increased by 1", discardCountBefore, state.discardCount[currentPlayer]);
    // verify that the Minion card was put in the player's discard pile
    assertEqual("[KNOWN BUG] Minion card added to player's discard pile", state.discard[currentPlayer][state.discardCount[currentPlayer]-1], minion);

    // verify minion card is no longer in the player's hand
    int minionCountAfter = 0;
    for (int i = 0; i < state.handCount[currentPlayer]; i++) {
        if (state.hand[currentPlayer][i] == minion) {
            minionCountAfter++;
        }
    }

    // verify the correct card was discarded
    assertEqual("The player's hand no longer contains a Minion card", minionCountAfter, 0);

    assertEqual("The player gained +2 coins", coinsBefore+2, state.coins);

}

int main() {

    minionTest1a();

    printf("[Minion Card test] Test complete\n");
    return 0;
}
