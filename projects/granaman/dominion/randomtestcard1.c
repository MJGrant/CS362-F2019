#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "my_utils.h"
#include "dominion.h"
#include <time.h>

// ************************
// Random tests for Baron card

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state

// cardBaron params: numPlayer, choice1, state
// choice1 is 0 = gain an estate (if there are any available)
// choice1 is 1 = discard an estate for +4 coins or gain an estate if you don't have one

void randomTestCard1a() {

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);

    int iteration = 0;

    // ********************************
    // Option 1: The player discards an estate from their hand for +4 coins
    // (choice1 = 1)

    // Preconditions:
    // randomize the size, cards of the player's hand
    // randomize the size, cards of the player's discard pile
    // randomize the quantity of estates in the estate pile

    // Postconditions:
    // Choice 1 is 1: the player gains +4 coins and discards an estate, or if the
    // player had no estates, the player gains one instead (if there are any in the supply)

    // cardBaron only returns 0, never -1
    // must test "failure" states some other way

    while(1) {
        iteration++;

        // randomize hand size and cards
        int randomHandCount = getRandomHandCount();
        state.handCount[currentPlayer] = randomHandCount;
        setRandomHand(&state, currentPlayer, randomHandCount);

        // randomize discard pile size and cards
        int randomDiscardCount = getRandomDiscardCount();
        state.discardCount[currentPlayer] = randomDiscardCount;
        setRandomDiscardPile(&state, currentPlayer, randomDiscardCount);

        setRandomEstateQuantity(&state);

        int handCountBefore = state.handCount[currentPlayer];
        int estateSupplyCountBefore = state.supplyCount[estate];
        int discardCountBefore = state.discardCount[currentPlayer];
        int topOfDiscardPileBefore = state.discard[currentPlayer][state.discardCount[currentPlayer]-1];

        int estateHandCountBefore = 0;
        for (int i = 0; i < state.handCount[currentPlayer]; i++) {
            if (state.hand[currentPlayer][i] == estate) {
                estateHandCountBefore++;
            }
        }

        int coinsBefore = state.coins;

        printf("Baron Random Test [Option 1: Discard an estate card for +4 coins]\nIteration #%d, hand count: %d, discard count: %d, estate supply pile: %d, estates in hand: %d \n", iteration, randomHandCount, discardCountBefore, estateSupplyCountBefore, estateHandCountBefore);

        // act - choice1 is 1, trade an estate (from hand) for +4 coins
        cardBaron(currentPlayer, 1, &state);

        if (estateHandCountBefore > 0) {
            // if the player actually has an estate card in hand to discard...
            assertEqual("Player gained +4 coins", coinsBefore+4, state.coins);
            assertIncreasedByOne("Player gained a card in their discard pile", discardCountBefore, state.discardCount[currentPlayer]);
            assertEqual("The top card in the discard pile is an estate", estate, state.discard[currentPlayer][state.discardCount[currentPlayer]-1]);
            assertDecreasedByOne("Hand count was reduced by one", handCountBefore, state.handCount[currentPlayer]);
        } else if (estateHandCountBefore == 0) {
            // if the player didn't have any estates in their hand, they should've attempted to draw one instead
            if (estateSupplyCountBefore > 0) {
                // if the supply pile had at least one estate to draw, confirm that it was successfully drawn
                assertEqual("The player's hand remains the same", handCountBefore, state.handCount[currentPlayer]);
                assertIncreasedByOne("Player gained a card in their discard pile", discardCountBefore, state.discardCount[currentPlayer]);
                assertEqual("The top card in the discard pile is an estate", estate, state.discard[currentPlayer][state.discardCount[currentPlayer]-1]);
            } else if (estateSupplyCountBefore == 0) {
                // but if the supply pile didn't have any, either, then the player should gain no coins
                // and draw no cards
                assertEqual("The player's hand remains the same", handCountBefore, state.handCount[currentPlayer]);
                assertEqual("Player's discard pile count is unchanged", discardCountBefore, state.discardCount[currentPlayer]);
                assertEqual("Top card of player's discard pile is unchanged", topOfDiscardPileBefore, state.discard[currentPlayer][state.discardCount[currentPlayer]-1]);
                assertEqual("Estate pile remains at zero", state.supplyCount[estate], 0);
            }
        }

        if (iteration == MAX_ITERATIONS) {
            printf("Done with %d iterations\n", MAX_ITERATIONS);
            break;
        }
    }
}

void randomTestCard1b() {

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);

    int iteration = 0;

    // ********************************
    // Option 2: The player wants to gain an estate
    // (choice1 = 0)

    // Preconditions:
    // randomize the size of the player's hand
    // randomize the cards in the player's hand
    // randomize the quantity of estates in the estate pile

    // Postconditions:
    // Choice 1 is 0: the player gains an estate and the estate pile is reduced by 1

    // The drawn card goes to the player's discard pile

    while(1) {
        iteration++;

        // randomize hand size and cards
        int randomHandCount = getRandomHandCount();
        state.handCount[currentPlayer] = randomHandCount;
        setRandomHand(&state, currentPlayer, randomHandCount);

        // randomize discard pile size and cards
        int randomDiscardCount = getRandomDiscardCount();
        state.discardCount[currentPlayer] = randomDiscardCount;
        setRandomDiscardPile(&state, currentPlayer, randomDiscardCount);

        setRandomEstateQuantity(&state);

        int handCountBefore = state.handCount[currentPlayer];
        int estateSupplyCountBefore = state.supplyCount[estate];
        int discardCountBefore = state.discardCount[currentPlayer];
        int topOfDiscardPileBefore = state.discard[currentPlayer][state.discardCount[currentPlayer]-1];

        printf("Baron Random Test [Option 2: Draw an estate card]\nIteration #%d, hand count: %d, discard count: %d, estate supply pile: %d \n", iteration, randomHandCount, discardCountBefore, estateSupplyCountBefore);

        // act - choice1 is 0, take an estate (if one exists in the supply)
        cardBaron(currentPlayer, 0, &state);

        assertEqual("The player's hand remains the same", handCountBefore, state.handCount[currentPlayer]);

        if (estateSupplyCountBefore > 0) {
            // if the supply count for estates happened to be 0, test that the
            // player didn't gain anything
            assertIncreasedByOne("Player gained a card in their discard pile", discardCountBefore, state.discardCount[currentPlayer]);
            assertEqual("The top card in the discard pile is an estate", estate, state.discard[currentPlayer][state.discardCount[currentPlayer]-1]);
            assertDecreasedByOne("Estate pile was reduced by one", estateSupplyCountBefore, state.supplyCount[estate]);
        } else if (estateSupplyCountBefore == 0) {
            assertEqual("Player's discard pile count is unchanged", discardCountBefore, state.discardCount[currentPlayer]);
            assertEqual("Top card of player's discard pile is unchanged", topOfDiscardPileBefore, state.discard[currentPlayer][state.discardCount[currentPlayer]-1]);
            assertEqual("Estate pile remains at zero", state.supplyCount[estate], 0);
        }

        if (iteration == MAX_ITERATIONS) {
            printf("Done with %d iterations\n", MAX_ITERATIONS);
            break;
        }
    }
}


int main() {
    srand(time(NULL));
    randomTestCard1a();
    randomTestCard1b();
    return 0;
}
