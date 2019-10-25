#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "dominion.h"
#include <time.h>

// ************************
// Unit test for Baron card
// Tests:

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state

// cardBaron params: numPlayer, choice1, state
// choice1 is 1 = discard an estate or gain an estate
// choice1 is 0 = gain an estate (if there are any available)

void printTestName(char *testName, char *testString) {
    printf("[%s] - %s\n", testName, testString);
}

int main() {

    printTestName("Baron Card", "Elect to gain an estate card");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);

    int handCountBefore = state.handCount[currentPlayer];
    int discardCountBefore = state.discardCount[currentPlayer];
    int numBuysBefore = state.numBuys; // save: numBuys
    int estateSupplyBefore = state.supplyCount[estate]; //save: how many estates are in the supply

    // act
    cardBaron(currentPlayer, 0, &state);

    printf("hand count: %d\n", state.handCount[currentPlayer]);

    // asserts

    // verify that the player got +1 buy for playing this card
    // note/possible bug: the +1 Buy never seems to get spent but it's also not required that it be spent
    assertEqual("Player gains +1 Buy", state.numBuys, numBuysBefore+1);

    // verify that the player gained one estate card
    assertEqual("Hand count increased by 1", state.handCount[currentPlayer], handCountBefore+1);

    // verify that one estate card was removed from the supply
    // FAILS because line 725 erroneously sets state->supplyCount[estate] = 0;
    assertEqual("Supply pile estate count decreased by 1", state.supplyCount[estate], estateSupplyBefore-1);

    // verify that the player's discard pile count went up by one
    assertEqual("Discard count went up by 1", state.discardCount[currentPlayer], discardCountBefore+1);

    // verify that an estate card was added to the last FILLED position of the player's deck
    assertEqual("Estate card added to player's discard pile", state.discard[currentPlayer][state.discardCount[currentPlayer]-1], estate);


    // ***********************
    printTestName("Baron Card", "Elect to gain an estate card");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);

    int handCountBefore = state.handCount[currentPlayer];
    int discardCountBefore = state.discardCount[currentPlayer];
    int numBuysBefore = state.numBuys; // save: numBuys
    int estateSupplyBefore = state.supplyCount[estate]; //save: how many estates are in the supply

    // act
    cardBaron(currentPlayer, 0, &state);





    printf("[Baron Card test] Test complete\n");
    return 0;
}
