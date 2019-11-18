#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "dominion.h"
#include <time.h>

// ************************
// Unit tests for Minion card

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state

// OLD cardMinion params: int currentPlayer, int choice1, int choice2, struct gameState *state, int handPos
// NEW minionRefactor params: int choice1, int choice2, struct gameState *state, int handPos

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
    minionRefactor(currentPlayer, 1, 0, &state, 1);

    // assert
    assertIncreasedByOne("The player gained an action", numActionsBefore, state.numActions);

    // verifying that the discard method was used properly
    // some of these FAIL because the discardCard method is full of problems
    // verify that the player's hand count decreased by one
    assertDecreasedByOne("The player's hand size has decreased by one", handCountBefore, state.handCount[currentPlayer]);
    // verify that the player's discard pile count went up by 1
    assertIncreasedByOne("[EXISTING BUG] Player's discard pile count increased by 1", discardCountBefore, state.discardCount[currentPlayer]);
    // verify that the Minion card was put in the player's discard pile
    assertEqual("[EXISTING BUG] Minion card added to player's discard pile", state.discard[currentPlayer][state.discardCount[currentPlayer]-1], minion);

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

void minionTest2a() {
    printTestName("Minion Card", "Player uses Minion to discard their entire hand as well as the hand of their opponent, who has >4 cards in hand");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);

    int discardCountBefore = state.discardCount[currentPlayer];
    int numActionsBefore = state.numActions;
    int coinsBefore = state.coins;

    // set the player's hand to a specific arrangement of cards
    // these are going to get discarded and replaced with a hand full of coppers
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = baron;
    state.hand[currentPlayer][1] = minion;
    state.hand[currentPlayer][2] = estate;
    state.hand[currentPlayer][3] = silver;

    // set the player's deck so they have some cards to draw
    state.deckCount[currentPlayer] = 6;
    state.deck[currentPlayer][0] = copper;
    state.deck[currentPlayer][1] = copper;
    state.deck[currentPlayer][2] = copper;
    state.deck[currentPlayer][3] = copper;
    state.deck[currentPlayer][4] = copper;
    state.deck[currentPlayer][5] = copper;

    // act
    // set choice1 (2nd param) to 1 (true) to use the "get 2 coins" card option
    // last param is "handPos", the position of the minion card in the player's hand
    minionRefactor(currentPlayer, 0, 1, &state, 1);

    // assert
    assertIncreasedByOne("The player gained an action", numActionsBefore, state.numActions);

    // verify the player's hand has 4 cards in it
    assertEqual("[MY BUG + EXISTING BUG] Player's new hand has 4 cards in it", 4, state.handCount[currentPlayer]);

    // verify the player's hand is all coppers now
    int copperCount = 0;
    for (int i = 0; i < state.deckCount[currentPlayer]; i++) {
        if (state.deck[currentPlayer][i] == copper) {
            printf("found a copper\n");
            copperCount++;
        }
    }
    assertEqual("[EXISTING BUG] The player's new hand is all coppers", copperCount, 4);

    assertIncreasedByOne("[MY BUG] Discard pile count increased by one", discardCountBefore, state.discardCount[currentPlayer]);
    assertEqual("[MY BUG] Minion card added to player's discard pile", state.discard[currentPlayer][state.discardCount[currentPlayer]-1], minion);

    //verify opponent's (player 0's) hand count is now four as a result of having drawn four new cards
    assertEqual("Opponent has 4 cards in hand", 4, state.handCount[0]);
    assertEqual("The player did not gain +2 coins", coinsBefore, state.coins);
}

void minionTest2b() {
    printTestName("Minion Card", "Player uses Minion to discard their entire hand as well as the hand of their opponent, who only has 3 cards in hand");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);


    // this test just looks at the state of the opponent's hand when the opponent only went into this round with
    // 3 cards in hand

    state.handCount[0] = 3;

    // act
    minionRefactor(currentPlayer, 0, 1, &state, 1);

    // assert
    assertEqual("Opponent was not forced to re-draw, still has 3 cards in hand", 3, state.handCount[0]);
}

int main() {

    minionTest1a();
    minionTest2a();
    minionTest2b();

    printf("[Minion Card test] Test complete\n");
    return 0;
}
