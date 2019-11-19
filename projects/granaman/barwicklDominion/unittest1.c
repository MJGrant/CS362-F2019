#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "dominion.h"
#include <time.h>

// ************************
// Unit tests for Baron card

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state

// OLD cardBaron params: numPlayer, choice1, state
// NEW baronRefactor params: int card, int choice1, struct gameState *state

// choice1 is 1 = discard an estate or gain an estate
// choice1 is 0 = gain an estate (if there are any available)

void baronTest1a() {
    printTestName("Baron Card", "Player draws an estate card from the stocked estate supply pile");

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
    // passing a zero as the second param indicates user wants to gain an estate
    // cardBaron(currentPlayer, 0, &state);

    // NEW baronRefactor params: int card, int choice1, struct gameState *state
    // note: the first param doesn't seem to be used for anything so I'll just pass copper...
    baronRefactor(copper, 0, &state);

    // assert

    // verify that the player got +1 buy for playing this card
    // note/possible bug: the +1 Buy never seems to get spent but it's also not required that it be spent
    assertIncreasedByOne("Player gains +1 Buy", numBuysBefore, state.numBuys);

    // verify that one estate card was removed from the supply
    // FAILS because line 725 erroneously sets state->supplyCount[estate] = 0;
    assertDecreasedByOne("[MY BUG] Estate supply pile count decreased by 1", estateSupplyBefore, state.supplyCount[estate]);

    // verify that the player's discard pile count went up by 1
    assertIncreasedByOne("Player's discard pile count increased by 1", discardCountBefore, state.discardCount[currentPlayer]);

    // verify that an estate card was added to the last position of the player's deck
    assertEqual("Estate card added to player's discard pile", state.discard[currentPlayer][state.discardCount[currentPlayer]-1], estate);
}

void baronTest1b() {
    printTestName("Baron Card", "Player draws estate card from the empty estate supply pile");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);
    state.whoseTurn = currentPlayer;

    int coinsBefore = state.coins;
    int discardCountBefore = state.discardCount[currentPlayer];
    int numBuysBefore = state.numBuys; // save: numBuys
    state.supplyCount[estate] = 0; // set supply pile for estates to zero
    int estateSupplyBefore = state.supplyCount[estate]; // record that zero

    // set the player's hand to a specific arrangement of cards
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = baron;
    state.hand[currentPlayer][1] = estate; // one estate in hand but shouldn't affect outcome
    state.hand[currentPlayer][2] = copper;
    state.hand[currentPlayer][3] = baron;
    int estateCountBefore = 1;

    int handCountBefore = state.handCount[currentPlayer];

    // act
    // passing a zero as the second param indicates user wants to gain an estate
    // cardBaron(currentPlayer, 0, &state);
    // note: the first param doesn't seem to do anything
    baronRefactor(copper, 0, &state);

    // assert
    // verify that the player did not get an estate card
    int estateCountAfter = 0;
    for (int i = 0; i < state.handCount[currentPlayer]; i++) {
        if (state.hand[currentPlayer][i] == estate) {
            estateCountAfter++;
        }
    }

    // verify that the player got +1 buy for playing this card
    // note/possible bug: the +1 Buy never seems to get spent but it's also not required that it be spent
    assertIncreasedByOne("Player gains +1 Buy", numBuysBefore, state.numBuys);

    assertEqual("Player did not gain an estate card", estateCountBefore, estateCountAfter);

    // verify that the player didn't gain or lose any other cards in their hand
    assertEqual("Player did not gain any cards in their hand", handCountBefore, state.handCount[currentPlayer]);

    // verify that the player didn't gain or lose any cards in their deck
    assertEqual("Player did not gain any cards in their discard pile", discardCountBefore, state.discardCount[currentPlayer]);

    assertEqual("Estate supply count remains the same", estateSupplyBefore, state.supplyCount[estate]);
    assertEqual("Estate supply count is 0", estateSupplyBefore, 0);

    // verify that the player did not collect +4 coins because they spent this turn on getting an estate instead
    assertEqual("Current player did not receive +4 coins", coinsBefore, state.coins);


}

void baronTest2a() {

    printTestName("Baron Card", "Player discards one estate card from her hand for +4 coins");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);
    state.whoseTurn = currentPlayer;

    // set the player's hand to a specific arrangement of cards
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = baron;
    state.hand[currentPlayer][1] = estate; // the loop should find and remove this estate
    state.hand[currentPlayer][2] = estate;
    state.hand[currentPlayer][3] = baron;

    int estateCountBefore = 2;

    int coinsBefore = state.coins;
    int handCountBefore = state.handCount[currentPlayer];
    int discardCountBefore = state.discardCount[currentPlayer];

    // act
    // passing a 1 as the middle param indicates user wants to trade an estate for 4 gold
    //baronRefactor(currentPlayer, 1, &state);

    // note: the first param doesn't seem to do anything
    baronRefactor(copper, 1, &state);

    // assert

    // verify that the user gained +4 gold
    assertEqual("Current player received +4 coins", state.coins, coinsBefore+4);

    // verify that the player's discard pile count went up by 1
    assertIncreasedByOne("Player's discard pile count increased by 1", discardCountBefore, state.discardCount[currentPlayer]);

    // verify that the Estate card was put in the player's discard pile
    assertEqual("Estate card added to player's discard pile", state.discard[currentPlayer][state.discardCount[currentPlayer]-1], estate);

    // verify that the player's hand count is decreased by 1
    assertDecreasedByOne("Player's hand count decreases by one", handCountBefore, state.handCount[currentPlayer]);

    // verify that an Estate card was removed from the player's hand
    int estateCountAfter = 0;
    for (int i = 0; i < state.handCount[currentPlayer]; i++) {
        if (state.hand[currentPlayer][i] == estate) {
            estateCountAfter++;
        }
    }

    // FAILS because the loop that fills the gap left from removing
    // an estate card doesn't start at the gap, it just starts at 0 and moves all cards left one
    // [baron, estate, estate, baron] becomes [estate, estate, baron]
    // could use more testing, this could have easily gone undetected given fortuitious placement of test cards
    assertDecreasedByOne("[EXISTING BUG] Player's hand has one fewer Estate cards in it", estateCountBefore, estateCountAfter);
}

void baronTest2b1() {
    printTestName("Baron Card", "Player discards one estate card from her hand for +4 coins when the player has no estates but the supply pile does");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);
    state.whoseTurn = currentPlayer;

    // set the player's hand to a specific arrangement of cards
    // player doesn't have any estate cards
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = baron;
    state.hand[currentPlayer][1] = copper;
    state.hand[currentPlayer][2] = copper;
    state.hand[currentPlayer][3] = baron;

    int coinsBefore = state.coins;
    int handCountBefore = state.handCount[currentPlayer];
    int discardCountBefore = state.discardCount[currentPlayer];

    int estateSupplyBefore = state.supplyCount[estate];

    // act
    // passing a 1 as the middle param indicates user wants to trade an estate for 4 gold
    // cardBaron(currentPlayer, 1, &state);

    // note: the first param doesn't seem to do anything
    baronRefactor(copper, 1, &state);

    // assert
    // verify that the player gained one estate card and the supply pile lost one estate
    // FAILS because supply count is being decreased twice, once in baronRefactor and once in gainCard
    assertDecreasedByOne("[EXISTING BUG] Estate supply pile count decreased by 1", estateSupplyBefore, state.supplyCount[estate]);

        // verify that the player didn't gain or lose any other cards in their hand
    assertEqual("Player did not gain any cards in their hand", handCountBefore, state.handCount[currentPlayer]);

    assertIncreasedByOne("Player's discard pile count increased by 1", discardCountBefore, state.discardCount[currentPlayer]);
    assertEqual("Estate card added to player's discard pile", state.discard[currentPlayer][state.discardCount[currentPlayer]-1], estate);

    // verify that the player did not collect +4 coins because they spent this turn on getting an estate instead
    assertEqual("Current player did not receive +4 coins", coinsBefore, state.coins);
}

void baronTest2b2() {
    printTestName("Baron Card", "Attempt to trade an estate card for +4 coins when neither the player nor the supply pile has an estate to give");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);
    state.whoseTurn = currentPlayer;

    // set the player's hand to a specific arrangement of cards
    // player doesn't have any estate cards
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = baron;
    state.hand[currentPlayer][1] = copper;
    state.hand[currentPlayer][2] = copper;
    state.hand[currentPlayer][3] = baron;

    // set the supply of estates to zero
    state.supplyCount[estate] = 0;

    int coinsBefore = state.coins;
    int handCountBefore = state.handCount[currentPlayer];
    int discardCountBefore = state.discardCount[currentPlayer];
    int estateSupplyBefore = state.supplyCount[estate];

    // act
    // passing a 1 as the second param indicates user wants to trade an estate for 4 gold
    // but in this case the player does not have an estate, nor does the supply pile
    // cardBaron(currentPlayer, 1, &state);

    // note: the first param doesn't seem to do anything
    baronRefactor(copper, 1, &state);

    // assert
    // verify that the player did not get an estate card
    int estateCountAfter = 0;
    for (int i = 0; i < state.handCount[currentPlayer]; i++) {
        if (state.hand[currentPlayer][i] == estate) {
            estateCountAfter++;
        }
    }
    assertEqual("Player did not gain an estate card", 0, estateCountAfter);

    // verify that the player didn't gain or lose any other cards in their hand
    assertEqual("Player did not gain any cards in their hand", handCountBefore, state.handCount[currentPlayer]);

    // verify that the player didn't gain or lose any cards in their deck
    assertEqual("Player did not gain any cards in their discard pile", discardCountBefore, state.discardCount[currentPlayer]);

    assertEqual("Estate supply count remains the same", estateSupplyBefore, state.supplyCount[estate]);
    assertEqual("Estate supply count is 0", estateSupplyBefore, 0);

    // verify that the player did not collect +4 coins because they spent this turn on getting an estate instead
    assertEqual("Current player did not receive +4 coins", coinsBefore, state.coins);

}


int main() {

    baronTest1a();
    baronTest1b();
    baronTest2a();
    baronTest2b1();
    baronTest2b2();

    printf("[Baron Card test] Test complete\n");
    return 0;
}
