#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "dominion.h"
#include <time.h>

#define NUM_PLAYERS 2

// helper method for determining what type of card a given card is
int getCardType(int card) {
    int ret = -1;
    if (card == copper || card == silver || card == gold) {
        // treasure card
        ret = 1;
    } else if (card == estate || card == duchy || card == province || card == gardens) {
        // victory card
        ret = 2;
    } else if (card == adventurer || card == council_room || card == feast || card == mine
        || card == remodel || card == smithy || card == village || card == baron || card == minion
        || card == steward || card == tribute || card == ambassador || card == cutpurse || card == embargo
        || card == outpost || card == salvager || card == sea_hag || card == treasure_map) {
        // action card
        ret = 3;
    } else if (card == great_hall) {
        //combo action-victory card
        ret = 4;
    } else if (card == curse) {
        // curse card
        ret = 5;
    }
    return ret;
}


// ************************
// Unit tests for bugs 1, 5, 9, 11

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state
// cardEffect(int card, int choice1, int choice2, int choice3, struct gameState *state, int handPos, int *bonus)

void bug1() {
    printTestName("Bug 1", "[Mine Card] Using the Mine card to trash a Treasure card should trash the Treasure card");

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
    printTestName("Bug 5 - a", "[Scoring] When calculating player’s final score, all cards in the player's deck are counted");

    // simulates what happens when there are more cards in the deck than in the discard pile

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(NUM_PLAYERS, k, 2, &state);

    // arrange current player's hand, deck, and discard piles
    state.handCount[currentPlayer] = 3;
    state.hand[currentPlayer][0] = estate;   // +1 to score
    state.hand[currentPlayer][1] = duchy;    // +3 to score
    state.hand[currentPlayer][2] = province; // +6 to score    // sums to 10

    state.discardCount[currentPlayer] = 1;
    state.discard[currentPlayer][0] = province; // +6 to score

    state.deckCount[currentPlayer] = 3;
    state.deck[currentPlayer][0] = estate;   // +1 to score
    state.deck[currentPlayer][2] = duchy;    // +3 to score
    state.deck[currentPlayer][3] = province; // +6 to score

    // should sum to 26 but will likely sum to 17 if bug is unfixed
    int expectedScore = 26;

    // act
    int actualScore = scoreFor(currentPlayer, &state);

    assertEqual("Player's score was properly totaled when deck count > discard count", expectedScore, actualScore);
}


void bug9() {

    int playerHandCountBefore;
    int numActionsBefore;
    int coinsBefore;
    int testedType1 = 0;
    int testedType2 = 0;
    int testedType3 = 0;
    int testedType4 = 0;
    int testedType5 = 0;

    // bug 9:
    printTestName("Bug 9", "[Tribute Card] Revealed cards of the same type are not properly handled as duplicates");

    // First, test that every card's type is properly returned
    assertEqual("Copper is a Treasure card", getCardType(copper), 1);
    assertEqual("Silver is a Treasure card", getCardType(silver), 1);
    assertEqual("Gold is a Treasure card", getCardType(gold), 1);

    assertEqual("Estate is a Victory card", getCardType(estate), 2);
    assertEqual("Duchy is a Victory card", getCardType(duchy), 2);
    assertEqual("Province is a Victory card", getCardType(province), 2);
    assertEqual("Gardens is a Victory card", getCardType(estate), 2);

    assertEqual("Adventurer is an Action card", getCardType(adventurer), 3);
    assertEqual("Council Room is an Action card", getCardType(council_room), 3);
    assertEqual("Feast is an Action card", getCardType(feast), 3);
    assertEqual("Mine is an Action card", getCardType(mine), 3);
    assertEqual("Remodel is an Action card", getCardType(remodel), 3);
    assertEqual("Smithy is an Action card", getCardType(smithy), 3);
    assertEqual("Village is an Action card", getCardType(village), 3);
    assertEqual("Baron is an Action card", getCardType(baron), 3);
    assertEqual("Minion is an Action card", getCardType(minion), 3);
    assertEqual("Steward is an Action card", getCardType(steward), 3);
    assertEqual("Tribute is an Action card", getCardType(tribute), 3);
    assertEqual("Ambassador is an Action card", getCardType(ambassador), 3);
    assertEqual("Cutpurse is an Action card", getCardType(cutpurse), 3);
    assertEqual("Embargo is an Action card", getCardType(embargo), 3);
    assertEqual("Outpost is an Action card", getCardType(outpost), 3);
    assertEqual("Salvager is an Action card", getCardType(salvager), 3);
    assertEqual("Sea Hag is an Action card", getCardType(sea_hag), 3);
    assertEqual("Treasure Map is an Action card", getCardType(treasure_map), 3);

    assertEqual("Great Hall is an Action-Victory card", getCardType(great_hall), 4);

    assertEqual("Curse is a Curse card", getCardType(curse), 5);

    // Now, test that every possible pairing of cards is correctly rewarded

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    int opponent = 0;
    initializeGame(NUM_PLAYERS, k, 2, &state);
    state.whoseTurn = currentPlayer;

    // act & assert

    // On each loop, the opponent has two cards in their deck to offer up as tribute cards.
    // The first one is the "i" card, the second one is the "j" card
    // Compare i and j to each other and if they are the same, run cardEffect so they
    // trigger the Tribute case and see if the player was properly rewarded.

    // Note: This isn't to test every possible combination of cards, it only looks for the first
    // instance of two cards being the same type, tests the outcome, and then does not test that type again.
    for (int i = 0; i < treasure_map; i++) {
        for (int j = 0; j < treasure_map; j++) {

            // arrange opponent and currentPlayer  back to starting state each loop
            state.numActions = 0;

            state.deckCount[opponent] = 2;
            state.deck[opponent][0] = i;
            state.deck[opponent][1] = j;

            state.handCount[currentPlayer] = 1;
            state.hand[currentPlayer][0] = tribute;

            playerHandCountBefore = state.handCount[currentPlayer];
            numActionsBefore = state.numActions;
            coinsBefore = state.coins;

            if (getCardType(i) == getCardType(j)) {
                int type = getCardType(i);
                // these cards are the same type!
                // run cardEffect and verify the outcome
                cardEffect(tribute, 0, 0, 0, &state, 0, 0);
                if (type == 1 && !testedType1) {
                    // if they are both treasure, expect +2 cards to be added to player's hand
                    assertEqual("[Two Treasure cards] player gained +2 coins", coinsBefore+2, state.coins);
                    testedType1 = 1;
                } else if (type == 2 && !testedType2) {
                    // if they are both victory, expect the player gained +2 cards in their hand
                    assertEqual("[Two Victory cards] player drew +2 cards to their hand", playerHandCountBefore+2, state.handCount[currentPlayer]);
                    testedType2 = 1;
                } else if (type == 3 && !testedType3) {
                    // if they are both action, expect that the player gained +2 actions
                    assertEqual("[Two Action cards] player gained +2 actions", numActionsBefore+2, state.numActions);
                    testedType3 = 1;
                } else if (type == 4 && !testedType4) {
                    // if they are both action-victory, expect +2 actions AND +2 cards drawn to hand
                    assertEqual("[Two Action-Victory cards] player gained +2 actions", numActionsBefore+2, state.numActions);
                    assertEqual("[Two Action-Victory cards] player had 2 cards and drew +2 cards to hand", playerHandCountBefore+2, state.handCount[currentPlayer]);
                    testedType4 = 1;
                } else if (type == 5 && !testedType5) {
                    // if they are both curse, expect no changes at all
                    assertEqual("[Two Curse cards] player drew no cards to hand", playerHandCountBefore, state.handCount[currentPlayer]);
                    assertEqual("[Two Curse cards] player gained no actions", numActionsBefore, state.numActions);
                    assertEqual("[Two Curse cards] player gained no coins", coinsBefore, state.coins);
                    testedType5 = 1;
                }
            }
        }
    } // ends outer loop

    // log some info about this run, helps ensure completeness
    if (testedType1 && testedType2 && testedType3 && testedType4 && testedType5) {
        printf("Tested all card types: Treasure, Victory, Action, Action-Victory, and Curse.\n");
    }
}

void bug11() {
    // bug 11:
    printTestName("Bug 11", "Bug #11: [Code Cleanup] [Minion Card]");
}


int main() {

    bug1();
    bug5();
    bug9();

    printf("[unittest_granaman] Test complete\n");
    return 0;
}
