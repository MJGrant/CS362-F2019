#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "my_asserts.h"
#include "dominion.h"
#include "dominion_helpers.h"
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
// Unit tests for bugs 6, 7, and 10

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state
// cardEffect(int card, int choice1, int choice2, int choice3, struct gameState *state, int handPos, int *bonus)

void bug6(int supplyCount, int cardToGain, char* cardName) {
    printTestName("Bug 6", "[Feast Card] The player should not gain additional bonus coins");
    printf("\t  Card to gain: %s\tCost: %d\tSupply: %d\n", cardName, getCost(cardToGain), supplyCount);

    // choice 1 is the supply # of the card gained
    // handPos is the position of the feast card itself

    // arrange
    struct gameState state;
    int k[10] = {adventurer, council_room, feast, gardens, mine,    //Set card array - include feast
                 remodel, smithy, village, ambassador, minion};
    int currentPlayer = 1;
    int randomSeed = rand();
    initializeGame(NUM_PLAYERS, k, randomSeed, &state);

    // *********************************
    // Play the feast card

    // arrange
    state.whoseTurn = currentPlayer;

    // arrange current player's cards
    // note: other players' cards not relevant to feast case
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = feast;
    state.hand[currentPlayer][1] = copper;              // each copper +1 coin
    state.hand[currentPlayer][2] = copper;              // state.coins should equal 3
    state.hand[currentPlayer][3] = copper;

    state.supplyCount[cardToGain] = supplyCount;        // set whether card to gain available in supply

    state.deckCount[currentPlayer] = 1;                 // use to check card gained to deck
    state.deck[currentPlayer][0] = baron;

    // save current variable states
    struct gameState pre;
    memcpy (&pre, &state, sizeof(struct gameState));

    // check values
//    printf("Coins: %d\n", state.coins);
//    printf("Supply count: %d\n", state.supplyCount[cardToGain]);

    // act
    // cardEffect(int card, int choice1, int choice2, int choice3, struct gameState *state, int handPos, int *bonus)
    // choice1 = choice 1 is the supply # of the card gained
    // note that the function is called with a bonus of 0 - no additional coins should be gained
    int ret = cardEffect(feast, cardToGain, 0, 0, &state, 0, 0);

    // tests for cardToGain was a valid choice (cost 0..5) and available in supply
    if (getCost(cardToGain) <= 5 && getCost(cardToGain >= 0) && supplyCount >= 1) {

        // asserts
        assertEqual("cardEffect returned successfully after played feast", ret, 0);
        assertEqual("Player does not gain additional coins", pre.coins, state.coins);

        // search for the gained card in the player's hand, discard, and deck; also played card pile
        int sentToDeck = 1;                                                         // assume true
        int cardInHand = 0, cardInDiscard = 0, cardInDeck = 0, cardInPlayed = 0;    // assume false
        for (int i = 0; i < state.handCount[currentPlayer]; i++)  {
            if (state.hand[currentPlayer][i] == cardToGain) {
                cardInHand = 1;
                sentToDeck = 0;
            }
        }
        for (int i = 0; i < state.discardCount[currentPlayer]; i++)  {
            if (state.discard[currentPlayer][i] == cardToGain) {
                cardInDiscard = 1;
                sentToDeck = 0;
            }
        }
        for (int i = 0; i < state.deckCount[currentPlayer]; i++)  {
            if (state.deck[currentPlayer][i] == cardToGain) {
                cardInDeck = 1;
            }
        }
        for (int i = 0; i < state.playedCardCount; i++)  {
            if (state.playedCards[i] == cardToGain) {
                cardInPlayed = 1;
                sentToDeck = 0;
            }
        }

        assertEqual("Gained card not in player's hand", cardInHand, 0);
        assertEqual("Gained card not in player's discard", cardInDiscard, 0);
        assertEqual("Gained card not in played card pile", cardInPlayed, 0);
        assertEqual("Gained card is in player's deck", cardInDeck, 1);

        printf("Final report:\n");
        assertEqual("Player did not gain additional coins from feast", pre.coins, state.coins);
        assertEqual("Gained card was sent to deck", sentToDeck, 1);
    }

    // tests for cardToGain was not valid choice (cost > 5 OR error condition of < 0)
    if (getCost(cardToGain) > 5 || getCost(cardToGain) < 0) {
        assertEqual("cardEffect returned error (-1) with invalid choice of card to gain", ret, -1);
    }

    // tests for cardToGain is not available in the supply
    if (supplyCount <= 0) {
        assertEqual("cardEffect returned error (-1) with none of card to gain in supply", ret, -1);
    }
}

void bug7(int numOpponentDeck, int opponentDeck[2], int numOpponentDiscard, int opponentDiscard[2]) {
    printTestName("Bug 7", "[Tribute Card] The for loop should not overrun the tributeRevealedCards array");

    // arrange
    struct gameState state;
    int k[10] = {adventurer, council_room, feast, gardens, mine,    //Set card array - include tribute
                 remodel, smithy, village, ambassador, tribute};
    int currentPlayer = 1;
    int nextPlayer = 0;
    int randomSeed = rand();
    initializeGame(NUM_PLAYERS, k, randomSeed, &state);

    // *********************************
    // Play the tribute card

    // arrange
    state.whoseTurn = currentPlayer;

    // arrange current player's cards
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = tribute;
    state.hand[currentPlayer][1] = copper;
    state.hand[currentPlayer][2] = copper;
    state.hand[currentPlayer][3] = copper;

    // arrange opponent's deck and discard (opponent's hand not used)
    state.deckCount[nextPlayer] = numOpponentDeck;
    for (int i = 0; i < state.deckCount[nextPlayer]; i++) {
        state.deck[nextPlayer][i] = opponentDeck[i];
//        printf("Deck: %d\n", state.deck[nextPlayer][i]);
    }

    state.discardCount[nextPlayer] = numOpponentDiscard;
    for (int i = 0; i < state.discardCount[nextPlayer]; i++) {
        state.discard[nextPlayer][i] = opponentDiscard[i];
//        printf("Discard: %d\n", state.discard[nextPlayer][i]);
    }

    // save current variable states
    struct gameState pre;
    memcpy (&pre, &state, sizeof(struct gameState));


    // act
    // cardEffect(int card, int choice1, int choice2, int choice3, struct gameState *state, int handPos, int *bonus)
    // tribute does not use the choice parameters

    cardEffect(tribute, 0, 0, 0, &state, 0, 0);
    int cardType1 = -1, cardType2 = -1;

    // tests for opponent has 0 cards in deck and 0 cards in discard
    // OR opponent reveals 2 curse cards in deck
    if ( (numOpponentDeck == 0 && numOpponentDiscard == 0) ||
         ( opponentDeck[0] == curse && opponentDeck[1] == curse )) {

        // asserts
        assertEqual("Actions not incremented", pre.numActions, state.numActions);
        assertEqual("Coins not incremented", pre.coins, state.coins);
        assertEqual("Cards not drawn", pre.handCount[currentPlayer], state.handCount[currentPlayer]);
        assertIncreasedByOne("Discard count incremented for played Tribute card", pre.discardCount[currentPlayer], state.discardCount[currentPlayer]);
    }

    // tests for reveals 1 Treasure (copper) card in deck
    cardType1 = getCardType(opponentDeck[0]);
    if ( numOpponentDeck == 1 && numOpponentDiscard == 0 && cardType1 == 1) {

        // asserts
        assertEqual("Actions not incremented", pre.numActions, state.numActions);
        assertEqual("Coins incremented by 2", pre.coins + 2, state.coins);
        assertEqual("Cards not drawn", pre.handCount[currentPlayer], state.handCount[currentPlayer]);
        assertIncreasedByOne("Discard count incremented for played Tribute card", pre.discardCount[currentPlayer], state.discardCount[currentPlayer]);
    }

    // tests for reveals 1 Victory (estate) in discard and 0 in deck
    cardType1 = getCardType(opponentDiscard[0]);
    if ( numOpponentDeck == 0 && numOpponentDiscard == 1 && cardType1 == 2) {

        // asserts
        assertEqual("Actions not incremented", pre.numActions, state.numActions);
        assertEqual("Coins not incremented", pre.coins, state.coins);
        assertEqual("2 cards drawn", pre.handCount[currentPlayer] + 1, state.handCount[currentPlayer]);  //-1 for discarded Tribute
        assertIncreasedByOne("Discard count incremented for played Tribute card", pre.discardCount[currentPlayer], state.discardCount[currentPlayer]);
    }

    // tests for reveals 2 of same Action (adventurer) in deck
    cardType1 = getCardType(opponentDeck[0]);
    cardType2 = getCardType(opponentDeck[1]);
    if ( numOpponentDeck == 2 && cardType1 == 3 && cardType2 == 3) {

        // asserts
        assertEqual("Actions incremented by 2", pre.numActions + 2, state.numActions);
        assertEqual("Coins not incremented", pre.coins, state.coins);
        assertEqual("Cards not drawn", pre.handCount[currentPlayer], state.handCount[currentPlayer]);
        assertIncreasedByOne("Discard count incremented for played Tribute card", pre.discardCount[currentPlayer], state.discardCount[currentPlayer]);
    }

    // tests for reveals 1 Action card and 1 Treasure card in deck
    cardType1 = getCardType(opponentDeck[0]);
    cardType2 = getCardType(opponentDeck[1]);
    if ( numOpponentDeck == 2 && cardType1 == 3 && cardType2 == 1) {

        // asserts
        assertEqual("Actions incremented by 2", pre.numActions + 2, state.numActions);
        assertEqual("Coins incremented by 2", pre.coins + 2, state.coins);
        assertEqual("Cards not drawn", pre.handCount[currentPlayer], state.handCount[currentPlayer]);
        assertIncreasedByOne("Discard count incremented for played Tribute card", pre.discardCount[currentPlayer], state.discardCount[currentPlayer]);
    }

}


int main() {

    srand(time(NULL));

    // Test feast card with cardToGain of cost < 5, == 5, and > 5
    bug6(1, smithy,   "Smithy");         // supply count of 1, smithy has a cost of 4
    bug6(1, tribute,  "Tribute");        // tribute has a cost of 5
//    bug6(1, province, "Province");     // province has a cost of 8 - infinte loop until dominion.c fixed
//    bug6(0, smithy, "Smithy");         // test card not available in supply - infinite loop until dominion.c fixed
//    bug6(1, -5, "Invalid Card");       // test invalid card to gain - infinite loop until dominion.c fixed

    // Test tribute card
    int opponentDeck[2] = {-1, -1};
    int opponentDiscard[2] = {-1, -1};

    printf("\nTest Tribute with 0 cards in deck and 0 cards in discard");
    bug7(0, opponentDeck, 0, opponentDiscard);

    printf("\nTest Tribute with 2 curse cards in deck");
    opponentDeck[0] = curse;
    opponentDeck[1] = curse;
    bug7(2, opponentDeck, 0, opponentDiscard);

    printf("\nTest Tribute with 1 Treasure (copper) in deck and 0 in discard");
    opponentDeck[0] = copper;
    opponentDeck[1] = -1;
    bug7(1, opponentDeck, 0, opponentDiscard);

    printf("\nTest Tribute with 1 Victory (estate) in discard and 0 in deck");
    opponentDeck[0] = -1;
    opponentDeck[1] = -1;
    opponentDiscard[0] = estate;
    bug7(0, opponentDeck, 1, opponentDiscard);

    printf("\nTest Tribute with 2 of same Action card in deck");
    opponentDeck[0] = adventurer;
    opponentDeck[1] = adventurer;
    opponentDiscard[0] = -1;
    bug7(2, opponentDeck, 0, opponentDiscard);

    printf("\nTest Tribute with 1 Action card and 1 Treasure card in deck");
    opponentDeck[0] = adventurer;
    opponentDeck[1] = silver;
    bug7(2, opponentDeck, 0, opponentDiscard);

    printf("\n[unittest_tayloreb] Tests complete\n");
    return 0;
}
