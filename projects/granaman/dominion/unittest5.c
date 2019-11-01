#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "dominion.h"
#include <time.h>

// ************************
// Unit tests for Mine card

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state
// cardMine params: int currentPlayer, int choice1, int choice2, struct gameState *state, int handPos
// choice 1 is the INDEX of card the player wants to trash FROM THEIR HAND
// choice 2 is the CARD (ie "silver") the player wants to get in exchange for choice1
// handPos is the position of the mine card itself, the player cannot trade the mine card itself

// note: these tests and the functionality they test will break if someone
// changes the order of cards in the enum list in dominion.h

void mineTest1() {

    printTestName("Mine Card", "Player plays the Mine card with a variety of acceptable card choices");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);
    int ret;

    int deckCountBefore, coinsBefore, numActionsBefore, handCountBefore;

    // *********************************
    // valid trade (copper for silver)
    // arrange
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = mine;
    state.hand[currentPlayer][1] = copper;
    state.hand[currentPlayer][2] = silver;
    state.hand[currentPlayer][3] = gold;

    handCountBefore = state.handCount[currentPlayer];
    deckCountBefore = state.deckCount[currentPlayer];

    // act
    ret = cardMine(currentPlayer, 1, silver, &state, 0);

    // assert
    assertEqual("Player is allowed to trade a copper for a silver", ret, 0);

    // verify the copper was removed from the player's hand
    int copperCount = 0;
    for (int i = 0; i < state.handCount[currentPlayer]; i++) {
        if (state.hand[currentPlayer][i] == copper) {
            copperCount++;
        }
    }

    assertEqual("Player's hand no longer contains the copper card that was traded", copperCount, 0);

    // verify the silver was added to the player's deck
    assertIncreasedByOne("[MY BUG] Player's hand count stays the same", handCountBefore, state.handCount[currentPlayer]);
    assertEqual("[MY BUG] Player's deck count did not change", deckCountBefore, state.deckCount[currentPlayer]);

    // *********************************
    // valid trade (silver for gold)
    // arrange
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = mine;
    state.hand[currentPlayer][1] = copper;
    state.hand[currentPlayer][2] = silver;
    state.hand[currentPlayer][3] = gold;

    handCountBefore = state.handCount[currentPlayer];
    deckCountBefore = state.deckCount[currentPlayer];

    // act
    ret = cardMine(currentPlayer, 2, gold, &state, 0);

    // assert
    assertEqual("Player is allowed to trade a silver for a gold", ret, 0);

    // verify the silver was removed from the player's hand
    int silverCount = 0;
    for (int i = 0; i < state.handCount[currentPlayer]; i++) {
        if (state.hand[currentPlayer][i] == silver) {
            silverCount++;
        }
    }

    assertEqual("Player's hand no longer contains the silver card that was traded", silverCount, 0);

    // verify the silver was added to the player's deck
    assertIncreasedByOne("[MY BUG] Player's hand count stays the same", handCountBefore, state.handCount[currentPlayer]);
    assertEqual("[MY BUG] Player's deck count did not change", deckCountBefore, state.deckCount[currentPlayer]);

    // *********************************
    // lateral and downgrade trades
    // arrange
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = mine;
    state.hand[currentPlayer][1] = copper;
    state.hand[currentPlayer][2] = silver;
    state.hand[currentPlayer][3] = gold;

    ret = cardMine(currentPlayer, 3, gold, &state, 0);
    assertEqual("[MY BUG] Player is allowed to make a lateral trade (gold for gold)", ret, 0);

    ret = cardMine(currentPlayer, 2, silver, &state, 0);
    assertEqual("[Player is allowed to make a lateral trade (silver for silver)", ret, 0);

    ret = cardMine(currentPlayer, 1, copper, &state, 0);
    assertEqual("[layer is allowed to make a lateral trade (copper for copper)", ret, 0);

    // arrange
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = mine;
    state.hand[currentPlayer][1] = copper;
    state.hand[currentPlayer][2] = silver;
    state.hand[currentPlayer][3] = gold;

    ret = cardMine(currentPlayer, 3, silver, &state, 0);
    assertEqual("[MY BUG] Player is allowed to trade down (gold for a silver)", ret, 0);

    ret = cardMine(currentPlayer, 3, silver, &state, 0);
    assertEqual("[EXISTING BUG] Player is allowed to trade down (silver for a copper)", ret, 0);

}


void mineTest2() {

    printTestName("Mine Card", "Player plays the Mine card with a variety of unacceptable card choices");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);
    int ret;

    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = mine;
    state.hand[currentPlayer][1] = province;
    state.hand[currentPlayer][2] = adventurer;
    state.hand[currentPlayer][3] = curse;

    int deckCountBefore, coinsBefore, numActionsBefore, handCountBefore;

    // act and assert
    ret = cardMine(currentPlayer, 1, silver, &state, 0);
    assertEqual("Player is prevented from trading a province for a silver", ret, -1);

    ret = cardMine(currentPlayer, 2, silver, &state, 0);
    assertEqual("Player is prevented from trading a adventurer for a silver", ret, -1);

    ret = cardMine(currentPlayer, 3, curse, &state, 0);
    assertEqual("Player is prevented from trading a curse for a silver", ret, -1);

    // arrange again, this time with good cards in-hand but this time,
    // the player will attempt to trade for cards they can't trade for

    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = mine;
    state.hand[currentPlayer][1] = copper;
    state.hand[currentPlayer][2] = silver;
    state.hand[currentPlayer][3] = gold;

    // act and assert
    // note: these tests and the functionality they test will break if someone
    // changes the order of cards in the enum list in dominion.h
    ret = cardMine(currentPlayer, 1, province, &state, 0);
    assertEqual("Player is prevented from trading a copper for a province", ret, -1);

    ret = cardMine(currentPlayer, 1, treasure_map, &state, 0);
    assertEqual("Player is prevented from trading a silver for a treasure map", ret, -1);

    ret = cardMine(currentPlayer, 3, province, &state, 0);
    assertEqual("Player is prevented from trading a gold for an adventurer", ret, -1);

    int negativeCard = -1;
    ret = cardMine(currentPlayer, 1, negativeCard, &state, 0);
    assertEqual("Player is prevented from trading a copper for a card that does not exist (low end)", ret, -1);

    int outOfBoundsCard = 27;
    ret = cardMine(currentPlayer, 1, outOfBoundsCard, &state, 0);
    assertEqual("Player is prevented from trading a copper for a card that does not exist (high end)", ret, -1);
}

int main() {
    mineTest1();
    mineTest2();

    printf("[Mine Card test] Test complete\n");
    return 0;
}
