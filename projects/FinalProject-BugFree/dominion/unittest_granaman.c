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

        if (i == currentPlayer) {
            assertIncreasedByOne("Player's discarded card count increases by 1 (mine card discarded)", discardCountBefore[i], discardCountAfter[i]);
        } else {
            assertEqual("Player's discard count remains the same", discardCountBefore[i], discardCountAfter[i]);
        }

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

    assertIncreasedByOne("Played cards count increases by one", playedCardCountBefore, playedCardCountAfter);

    // verify it was placed in the played card pile (we need to keep record of)
    int copperCountInPlayed = 0;
    for (int i = 0; i < state.playedCardCount; i++) {
        if (state.playedCards[i] == copper) {
            checked++;
            passing++;
            copperCountInPlayed++;
        }
    }
    assertEqual("Copper card (card #4) was found in played cards pile", copperCountInPlayed, 1);

    printf("Final report:\n");
    assertEqual("Copper card was trashed", checked, passing);

}

void bug5() {
    // bug 5:
    printTestName("Bug 5", "[Scoring] When calculating player’s final score, all cards in the player's deck are counted");

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
    state.deck[currentPlayer][1] = duchy;    // +3 to score
    state.deck[currentPlayer][2] = province; // +6 to score

    // should sum to 26 but will likely sum to 17 if bug is unfixed
    int expectedScore = 26;

    // act
    int actualScore = scoreFor(currentPlayer, &state);

    assertEqual("Player's score was properly totaled when deck count > discard count", expectedScore, actualScore);

    // arrange the opposite scenario, discard count > deck count
    struct gameState state2;
    initializeGame(NUM_PLAYERS, k, 2, &state2);

    // arrange current player's hand, deck, and discard piles
    state2.handCount[currentPlayer] = 3;
    state2.hand[currentPlayer][0] = estate;   // +1 to score
    state2.hand[currentPlayer][1] = duchy;    // +3 to score
    state2.hand[currentPlayer][2] = province; // +6 to score    // sums to 10

    state2.discardCount[currentPlayer] = 3;
    state2.discard[currentPlayer][0] = province; // +6 to score
    state2.discard[currentPlayer][1] = estate; // +1 to score
    state2.discard[currentPlayer][2] = duchy; // +3 to score

    state2.deckCount[currentPlayer] = 1;
    state2.deck[currentPlayer][0] = estate;   // +1 to score

    // should sum to 21 but will likely sum to 22 because the card in state2.deck[currentPlayer][1] is being counted
    expectedScore = 21;

    // act
    actualScore = scoreFor(currentPlayer, &state2);

    assertEqual("Player's score was properly totaled when discard count > deck count", expectedScore, actualScore);
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

    // stuff the player's deck full of cards so these tests have enough to draw from
    state.deckCount[currentPlayer] = 100;
    for (int i = 0; i < state.deckCount[currentPlayer]; i++) {
        state.deck[currentPlayer][i] = copper;
    }

    // act & assert

    // On each loop, the opponent has two cards in their deck to offer up as tribute cards.
    // The first one is the "i" card, the second one is the "j" card
    // Compare i and j to each other and if they are the same, run cardEffect so they
    // trigger the Tribute case and see if the player was properly rewarded.

    // Note: This isn't to test every possible combination of cards, it only looks for the first
    // instance of two cards being the same type, tests the outcome, and then does not test that type again.
    for (int i = 0; i < treasure_map; i++) {
        for (int j = 0; j < treasure_map; j++) {
            // arrange opponent and currentPlayer back to starting state each loop

            state.deckCount[opponent] = 2;
            state.deck[opponent][0] = i;
            state.deck[opponent][1] = j;

            state.handCount[currentPlayer] = 1;
            state.hand[currentPlayer][0] = tribute;

            if (getCardType(i) == getCardType(j)) {
                int type = getCardType(i);
                // these cards are the same type!
                // run cardEffect and verify the outcome
                playerHandCountBefore = state.handCount[currentPlayer];
                numActionsBefore = state.numActions;
                coinsBefore = state.coins;
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
                    assertEqual("[Two Action-Victory cards] player drew +2 cards to hand", playerHandCountBefore+2, state.handCount[currentPlayer]);
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

void bug11a() {
    // bug 11:
    printTestName("Bug 11", "Bug #11: [Code Cleanup] [Minion Card regression test]");

    printTestName("Minion Card, branch 1", "Player discards Minion and gains +2 coins");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 0;
    initializeGame(2, k, 2, &state);
    state.whoseTurn = currentPlayer;

    int discardCountBefore = state.discardCount[currentPlayer];

    // set the player's hand to a specific arrangement of cards
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = baron;
    state.hand[currentPlayer][1] = minion; // one estate in hand but shouldn't affect outcome
    state.hand[currentPlayer][2] = copper; // worth 1
    state.hand[currentPlayer][3] = copper; // worth 1
    int handCountBefore = state.handCount[currentPlayer];

    state.coins = 2;
    int coinsBefore = state.coins;

    // act
    // set choice1 (2nd param) to 1 (true) to use the "get 2 coins" card option
    // last param is "handPos", the position of the minion card in the player's hand

    // int playCard(int handPos, int choice1, int choice2, int choice3, struct gameState *state)
    // cardEffect(card, choice1, choice2, choice3, state, handPos, &coin_bonus)

    // changed to playCard on 12/3 to work with the new way bonus coins are added
    playCard(1, 1, 0, 0, &state);

    // assert

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

void bug11b() {
    printTestName("Minion Card, branch 2", "Player uses Minion to discard their entire hand as well as the hand of the opponent who has >4 cards in hand");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 0;
    int opponentDiscardHand = 1;
    int opponentNoDiscardHand = 2;
    int playerCount = 3;
    initializeGame(playerCount, k, 2, &state);
    state.whoseTurn = currentPlayer;

    // set the player's hand to a specific arrangement of cards
    // these are going to get discarded and replaced with a hand full of coppers
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = baron;
    state.hand[currentPlayer][1] = minion;
    state.hand[currentPlayer][2] = estate;
    state.hand[currentPlayer][3] = silver;

    // first opponent has 5 cards and should be forced to discard and redraw
    state.handCount[opponentDiscardHand] = 5;
    state.hand[opponentDiscardHand][0] = baron;
    state.hand[opponentDiscardHand][1] = estate;
    state.hand[opponentDiscardHand][2] = minion;
    state.hand[opponentDiscardHand][3] = silver;
    state.hand[opponentDiscardHand][4] = silver;

    // second opponent has 2 cards and will not be forced to discard and redraw
    state.handCount[opponentNoDiscardHand] = 2;
    state.hand[opponentNoDiscardHand][0] = baron;
    state.hand[opponentNoDiscardHand][1] = silver;

    // set the players' decks so they have some cards to draw
    state.deckCount[currentPlayer] = 6;
    state.deckCount[opponentDiscardHand] = 6;
    state.deckCount[opponentNoDiscardHand] = 6;
    for (int i = 0; i < playerCount; i++) {
        for (int j = 0; j < 6; j++) {
            state.deck[i][j] = copper;
        }
    }

    // save the "befores"
    int numActionsBefore = state.numActions;
    int coinsBefore = state.coins;

    int currentPlayerDiscardCountBefore = state.discardCount[currentPlayer];

    int opponentNoDiscardHandCountBefore = state.handCount[opponentNoDiscardHand];

    // act
    // set choice1 (2nd param) to 1 (true) to use the "get 2 coins" card option
    // cardEffect(card, choice1, choice2, choice3, state, handPos, &coin_bonus)
    int bonusVar = 0;
    cardEffect(minion, 0, 1, 0, &state, 1, &bonusVar);

    // assert
    assertIncreasedByOne("The player gained an action", numActionsBefore, state.numActions);
    assertEqual("The player did not gain +2 coins", coinsBefore, state.coins);

    // verify the player's hand has 4 cards in it
    assertEqual("Player's new hand has 4 cards in it", 4, state.handCount[currentPlayer]);
    // verify the player's hand is all coppers now
    int copperCount = 0;
    for (int i = 0; i < state.handCount[currentPlayer]; i++) {
        if (state.hand[currentPlayer][i] == copper) {
            copperCount++;
        }
    }
    assertEqual("The player's new hand is all coppers", copperCount, state.handCount[currentPlayer]);
    assertIncreasedByOne("[EXISTING BUG] Current player's discard pile count increased by one", currentPlayerDiscardCountBefore, state.discardCount[currentPlayer]);
    assertEqual("[EXISTING BUG] Minion card added to current player's discard pile", state.discard[currentPlayer][state.discardCount[currentPlayer]-1], minion);

    //verify "discard" opponent's hand count is now four as a result of having drawn four new cards
    assertEqual("Opponent who started with 5 cards now has 4 cards in hand", 4, state.handCount[opponentDiscardHand]);
    // those cards are all coppers
    copperCount = 0;
    for (int i = 0; i < state.handCount[opponentDiscardHand]; i++) {
        if (state.hand[opponentDiscardHand][i] == copper) {
            copperCount++;
        }
    }
    assertEqual("Opponent who started with 5 cards now has different cards in hand", copperCount, state.handCount[opponentDiscardHand]);

    //verify "no discard" opponent's hand count is still 3
    assertEqual("Opponent who started with 2 cards still has 2 cards in hand", opponentNoDiscardHandCountBefore, state.handCount[opponentNoDiscardHand]);
    // none of those cards are coppers (were not replaced)
    copperCount = 0;
    for (int i = 0; i < state.handCount[opponentNoDiscardHand]; i++) {
        if (state.hand[opponentNoDiscardHand][i] == copper) {
            copperCount++;
        }
    }
    assertEqual("Opponent who started with 2 cards did not replace those cards with cards from deck", copperCount, 0);
}


int main() {

    bug1();
    bug5();
    bug9();
    bug11a();
    bug11b();

    printf("[unittest_granaman] Test complete\n");
    return 0;
}