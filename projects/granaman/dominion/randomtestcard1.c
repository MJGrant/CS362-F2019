#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "dominion.h"
#include <time.h>

// ************************
// Random tests for Baron card

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state

// cardBaron params: numPlayer, choice1, state
// choice1 is 0 = gain an estate (if there are any available)
// choice1 is 1 = discard an estate for +4 coins or gain an estate if you don't have one

int MAX_ITERATIONS = 100;
int MAX_HAND_SIZE = 10; // up to 500 is allowed but realistically the player's hand size is rarely over 10

int getRandomHandCount() {
    int random = rand() % MAX_HAND_SIZE; // between 0 and max hand size
    return random;
}

int getRandomDiscardCount() {
    int random = rand() % MAX_DECK; // between 0 and 500 as defined in dominion.h
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

void setRandomDiscardPile(struct gameState *state, int currentPlayer, int randomDiscardCount) {
    for (int i = 0; i < randomDiscardCount; i++) {
        state->discard[currentPlayer][i] = getRandomCard();
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
    // Option 1: The player discards an estate from their hand for +4 coins
    // (choice1 = 1)

    // Preconditions:
    // randomize the size of the player's hand
    // randomize the cards in the player's hand
    // randomize the quantity of estates in the estate pile

    // Postconditions:
    // Choice 1 is 1: the player gains +4 coins and discards an estate, or if the
    // player had no estates, the player gains one instead

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

        printf("Baron Random Test [Option 1: Discard estate for +4 coins] - Iteration %d, hand count: %d, estate supply pile: %d \n", iteration, randomHandCount, state.supplyCount[estate]);

        // act - choice1 is 1; the player wants to discard an estate they have
        int coinsBefore = state.coins;
        cardBaron(currentPlayer, 1, &state);
        if (state.coins == coinsBefore + 4) {
            printf("coins ok!\n");
        }

        if (iteration == MAX_ITERATIONS) {
            printf("Done with %d iterations\n", MAX_ITERATIONS);
            exit(200);
        }
    }
}

void randomTestCard2() {

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
        int estateCountBefore = state.supplyCount[estate];
        int discardCountBefore = state.discardCount[currentPlayer];
        int topOfDiscardPileBefore = state.discard[currentPlayer][state.discardCount[currentPlayer]-1];
        printf("topOfDiscardPileBefore: %d\n", topOfDiscardPileBefore);

        printf("Baron Random Test [Option 2: Draw an estate card]\nIteration #%d, hand count: %d, discard count: %d, estate supply pile: %d \n", iteration, randomHandCount, discardCountBefore, estateCountBefore);

        // act - choice1 is 0, take an estate (if one exists in the supply)
        cardBaron(currentPlayer, 0, &state);

        assertEqual("The player's hand remains the same", handCountBefore, state.handCount[currentPlayer]);

        if (estateCountBefore == 0) {
            // if the supply count for estates happened to be 0, test that the
            // player didn't gain anything
            assertEqual("Player's discard pile count is unchanged", discardCountBefore, state.discardCount[currentPlayer]);
            assertEqual("Top card of player's discard pile is unchanged", topOfDiscardPileBefore, state.discard[currentPlayer][state.discardCount[currentPlayer]-1]);
            assertEqual("Estate pile remains at zero", state.supplyCount[estate], 0);
        } else {
            assertIncreasedByOne("Player gained a card in their discard pile", discardCountBefore, state.discardCount[currentPlayer]);
            assertEqual("The top card in the discard pile is an estate", estate, state.discard[currentPlayer][state.discardCount[currentPlayer]-1]);
            assertDecreasedByOne("Estate pile was reduced by one", estateCountBefore, state.supplyCount[estate]);
        }

        if (iteration == MAX_ITERATIONS) {
            printf("Done with %d iterations\n", MAX_ITERATIONS);
            exit(200);
        }
    }
}


int main() {
    srand(time(NULL));
    //randomTestCard1();
    randomTestCard2();
    return 0;
}
