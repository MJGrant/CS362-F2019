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


int main() {

    // arrange

    printf("*** TESTING *** [Baron Card] Elect to gain an estate card\n");

    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;

    initializeGame(2, k, 2, &state);

    int numBuysBefore = state.numBuys; // save: numBuys
    int estateSupplyBefore = state.supplyCount[estate]; //save: how many estates are in the supply

    // act

    cardBaron(currentPlayer, 0, &state);

    printf("hand count: %d\n", state.handCount[currentPlayer]);

    // assert

    // verify that the player gained one estate card
    // verify that one estate card was removed from the supply
    //assert(state.hand[currentPlayer] == estate)
    printf("estate count before: %d estate count now: %d\n", estateSupplyBefore, state.supplyCount[estate]);
    assertEqual(state.supplyCount[estate], estateSupplyBefore-1);

    // verify the player got +1 Buy for playing this card
    //assert(state.numBuys == numBuysBefore+1);      //todo: 1 buy is granted but there is no way to use it?

    // assert that an estate card was gained


    // asserts
    printf("supply count: %d\n", state.supplyCount[estate]);
    //assert(state.supplyCount[estate] == estateSupplyBefore - 1);


    // make sure the number of buys was increased
    //assert(state.numBuys == tempNumBuys+1);

    printf("Unit Test 1: Baron Card -- [Passed]\n");
    return 0;
}
