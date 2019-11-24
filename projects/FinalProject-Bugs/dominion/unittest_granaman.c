#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "dominion.h"
#include <time.h>

#define NUM_PLAYERS 2

// ************************
// Unit tests for bugs 1, 5, 9, 11

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state
// cardEffect(int card, int choice1, int choice2, int choice3, struct gameState *state, int handPos, int *bonus)

void bug1() {
    printTestName("Bug 1", "[Mine Card] Using the Mine card to trash a Treasure card does not actually trash the Treasure card");

    // choice 1 is the INDEX of card the player wants to trash FROM THEIR HAND
    // choice 2 is the CARD (ie "silver") the player wants to get in exchange for choice1
    // handPos is the position of the mine card itself, the player cannot trade the mine card itself

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(NUM_PLAYERS, k, 2, &state);

    // *********************************
    // Performs a valid trade (give away copper and get a silver)
    // Verify that the copper was not given to player's hand, deck, or discard pile
    // Also verify that the copper was not given to opponent's hand, deck, played, or discard pile
    // If it's in none of those places, it was truly trashed

    // arrange
    state.whoseTurn = currentPlayer;

    // arrange current player
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = mine; // enum 11
    state.hand[currentPlayer][1] = copper; // enum 4
    state.hand[currentPlayer][2] = silver; // enum 5
    state.hand[currentPlayer][3] = gold; // enum 6

    state.discardCount[currentPlayer] = 1;
    state.discard[currentPlayer][0] = duchy;

    state.deckCount[currentPlayer] = 1;
    state.deck[currentPlayer][0] = baron;

    state.playedCardCount = 3;
    state.playedCards[0] = baron;
    state.playedCards[1] = mine;
    state.playedCards[2] = gold;

    // arrange other players (just stuff their hands and decks with cards that aren't copper)
    int opponentCardCount = 2;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        if (i != currentPlayer) {
            // init everyone else's hand, deck, and discard to the count value
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
    }

    // record every player's "before" counts in these arrays
    // each index represents a player, ie: player 0 and player 1
    // yeah this step is kinda redundant but it's good to do it this way in case
    // we ever randomize the counts for each pile
    int handCountBefore[NUM_PLAYERS] = {0,0};
    int discardCountBefore[NUM_PLAYERS] = {0,0};
    int deckCountBefore[NUM_PLAYERS] = {0,0};

    int playedCardCountBefore = state.playedCardCount;

    for (int i = 0; i < NUM_PLAYERS; i++) {
        handCountBefore[i] = state.handCount[i];
        discardCountBefore[i] = state.discardCount[i];
        deckCountBefore[i] = state.deckCount[i];
    }

    // act
    // cardEffect(int card, int choice1, int choice2, int choice3, struct gameState *state, int handPos, int *bonus)
    // choice1 = index of card we want to discard (so 1 in this case, for copper)
    // choice2 = name of card we want to get (silver in this case)
    // copper has a value of "4" when logged as an int
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

    int playedCardCountAfter = state.playedCardCount;

    // verify that the copper card is not in anyone's hand, deck, or discard piles
    int passing = 0;
    int checked = 0;
    for (int i = 0; i < NUM_PLAYERS; i++) {
        printf("Checking player %d's hand, deck, and discard piles\n", i);

        // verify that totals remained unchanged
        if (i == currentPlayer) {
            assertEqual("Player's hand count decreases by one (copper trashed, silver added, mine discarded)", handCountBefore[i]-1, handCountAfter[i]);
        } else {
            assertEqual("Player's hand count remains the same", handCountBefore[i], handCountAfter[i]);
        }
        assertEqual("Player's deck count remains the same", deckCountBefore[i], deckCountAfter[i]);
        assertEqual("Player's discard count remains the same", discardCountBefore[i], discardCountAfter[i]);

        // check this player's hand
        for (int j = 0; j < state.handCount[i]; j++) {
            assertNotEqual("Copper card (card #4) not found in hand", state.hand[i][j], copper);
            checked++;
            if (state.hand[i][j] != copper) {
                passing++;
            }
        }

        // check ths player's deck
        for (int j = 0; j < state.deckCount[i]; j++) {
            assertNotEqual("Copper card (card #4) not found in deck", state.deck[i][j], copper);
            checked++;
            if (state.deck[i][j] != copper) {
                passing++;
            }
        }

        // check this player's discard
        for (int j = 0; j < state.discardCount[i]; j++) {
            assertNotEqual("Copper card (card #4) not found in discard pile", state.discard[i][j], copper);
            checked++;
            if (state.discard[i][j] != copper) {
                passing++;
            }
        }
    }

    printf("Checking played cards pile for a copper (card #4)\n");

    assertEqual("Played cards count remains the same", playedCardCountBefore, playedCardCountAfter);

    // also verify it was not played in the played card pile (shared by all players)
    for (int i = 0; i < state.playedCardCount; i++) {
        assertNotEqual("Copper card (card #4) not found in played cards pile", state.playedCards[i], copper);
        checked++;
        if (state.playedCards[i] != copper) {
            passing++;
        }
    }

    printf("Final report:\n");
    assertEqual("Copper card was trashed", checked, passing);

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
