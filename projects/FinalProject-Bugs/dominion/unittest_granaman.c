#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "dominion.h"
#include <time.h>

// ************************
// Unit tests for bugs 1, 5, 9, 11

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state

void bug1() {
    printTestName("Bug 1", "[Mine Card] Using the Mine card to trash a Treasure card does not actually trash the Treasure card");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);
    state.whoseTurn = currentPlayer;

    int discardCountBefore = state.discardCount[currentPlayer];
    int numBuysBefore = state.numBuys; // save: numBuys
    int estateSupplyBefore = state.supplyCount[estate]; //save: how many estates are in the supply

    // act

    // assert
}

void bug5() {
    // bug 5:
    printTestName("Bug 5", "[Scoring] Player’s final score is not properly totaled");
}

void bug9() {
    // bug 9:
    printTestName("Bug 9", "[Tribute Card] Revealed cards of the same type are not properly handled as duplicates");

}

void bug11() {
    // bug 11:
    printTestName("Bug 11", "Bug #11: [Code Cleanup] [Minion Card]");
}


int main() {

    bug1();

    printf("[unittest_granaman] Test complete\n");
    return 0;
}
