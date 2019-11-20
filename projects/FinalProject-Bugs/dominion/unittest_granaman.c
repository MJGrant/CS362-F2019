#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "dominion.h"
#include <time.h>

#define NUM_PLAYERS 2

// ************************
// Unit tests for bugs 1, 5, 9, 11

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state
// cardEffect params: cardEffect(card, choice1, choice2, choice3, state, handPos, &coin_bonus)

void bug1() {
    printTestName("Bug 1", "[Mine Card] Using the Mine card to trash a Treasure card does not actually trash the Treasure card");

    // choice 1 is the INDEX of card the player wants to trash FROM THEIR HAND
    // choice 2 is the CARD (ie "silver") the player wants to get in exchange for choice1
    // handPos is the position of the mine card itself, the player cannot trade the mine card itself

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int opponent = 0;
    int currentPlayer = 1;
    initializeGame(NUM_PLAYERS, k, 2, &state);

    // *********************************
    // Performs a valid trade (give away copper and get a silver)
    // Verify that the copper was not given to player's hand, deck, or discard pile
    // Also verify that the copper was not given to opponent's hand, deck, or discard pile
    // If it's in none of those places, it was truly trashed

    // arrange

    // arrange current player
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = mine;
    state.hand[currentPlayer][1] = copper;
    state.hand[currentPlayer][2] = silver;
    state.hand[currentPlayer][3] = gold;

    state.discardCount[currentPlayer] = 1;
    state.discard[currentPlayer][0] = duchy;

    state.deckCount[currentPlayer] = 1;
    state.deck[currentPlayer][0] = baron;

    // arrange other players (just stuff their hands and decks with cards that aren't copper)
    int opponentCardCount = 2;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        // init everyone's hand, deck, and discard to the count value
        state.handCount[i] = opponentCardCount;
        state.deckCount[i] = opponentCardCount;
        state.discardCount[i] = opponentCardCount;
        for (int j = 0; j < opponentCardCount; j++) {
            // fill 'em up with curse cards
            state.hand[i][j] = curse;
            state.deck[i][j] = curse;
            state.discard[i][j] = curse;
        }
    }

    // record every player's "before" counts in these arrays
    // each index represents a player, ie: player 0 and player 1
    // yeah this step is kinda redundant but it's good to do it this way in case
    // we ever randomize the counts for each pile
    int handCountBefore[NUM_PLAYERS] = {0,0};
    int discardCountBefore[NUM_PLAYERS] = {0,0};
    int deckCountBefore[NUM_PLAYERS] = {0,0};

    for (int i = 0; i < NUM_PLAYERS; i++) {
        handCountBefore[i] = state.handCount[i];
        discardCountBefore[i] = state.discardCount[i];
        deckCountBefore[i] = state.deckCount[i];
    }

    // act
    int ret = cardEffect(mine, 1, silver, 0, &state, 0, 0);

    // assert
    assertEqual("Player is allowed to trade a copper for a silver", ret, 0);

    // collect "after" snapshots for every player
    int handCountAfter[NUM_PLAYERS] = {0,0};
    int discardCountAfter[NUM_PLAYERS] = {0,0};
    int deckCountAfter[NUM_PLAYERS] = {0,0};

    for (int i = 0; i < NUM_PLAYERS; i++) {
        handCountAfter[i] = state.handCount[i];
        discardCountAfter[i] = state.discardCount[i];
        deckCountAfter[i] = state.deckCount[i];
    }

    // verify that the copper card is not in anyone's hand, deck, or discard piles
    int passing = 0;
    int pilesChecked = 0;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        printf("Checking player %d's hand, deck, and discard piles\n", i);
        for (int j = 0; j < state.handCount[i]; j++) {
            assertNotEqual("Copper card not found in hand", state.hand[i][j], copper);
            pilesChecked++;
            if (state.hand[i][j] != copper) {
                passing++;
            }
        }

        for (int j = 0; j < state.deckCount[i]; j++) {
            assertNotEqual("Copper card not found in deck", state.deck[i][j], copper);
            pilesChecked++;
            if (state.deck[i][j] != copper) {
                passing++;
            }
        }

        for (int j = 0; j < state.discardCount[i]; j++) {
            assertNotEqual("Copper card not found in discard pile", state.discard[i][j], copper);
            pilesChecked++;
            if (state.discard[i][j] != copper) {
                passing++;
            }
        }
    }


    assertEqual("Copper card was trashed", pilesChecked, passing);

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
