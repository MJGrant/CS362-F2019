#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "my_asserts.h"
#include "dominion.h"
#include "dominion_helpers.h"
#include <time.h>

#define NUM_PLAYERS 2

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
    }

    // tests for reveals 1 Treasure (copper) card in deck
    cardType1 = getCardType(opponentDeck[0]);
    if ( numOpponentDeck == 1 && numOpponentDiscard == 0 && cardType1 == 1) {

        // asserts
        assertEqual("Actions not incremented", pre.numActions, state.numActions);
        assertEqual("Coins incremented by 2", pre.coins + 2, state.coins);
        assertEqual("Cards not drawn", pre.handCount[currentPlayer], state.handCount[currentPlayer]);
    }

    // tests for reveals 1 Victory (estate) in discard and 0 in deck
    cardType1 = getCardType(opponentDiscard[0]);
    if ( numOpponentDeck == 0 && numOpponentDiscard == 1 && cardType1 == 2) {

        // asserts
        assertEqual("Actions not incremented", pre.numActions, state.numActions);
        assertEqual("Coins not incremented", pre.coins, state.coins);
        assertEqual("2 cards drawn", pre.handCount[currentPlayer] + 2, state.handCount[currentPlayer]);
    }

    // tests for reveals 2 of same Action (adventurer) in deck
    cardType1 = getCardType(opponentDeck[0]);
    cardType2 = getCardType(opponentDeck[1]);
    if ( numOpponentDeck == 2 && cardType1 == 3 && cardType2 == 3) {

        // asserts
        assertEqual("Actions incremented by 2", pre.numActions + 2, state.numActions);
        assertEqual("Coins not incremented", pre.coins, state.coins);
        assertEqual("Cards not drawn", pre.handCount[currentPlayer], state.handCount[currentPlayer]);
    }

    // tests for reveals 1 Action card and 1 Treasure card in deck
    cardType1 = getCardType(opponentDeck[0]);
    cardType2 = getCardType(opponentDeck[1]);
    if ( numOpponentDeck == 2 && cardType1 == 3 && cardType2 == 1) {

        // asserts
        assertEqual("Actions incremented by 2", pre.numActions + 2, state.numActions);
        assertEqual("Coins incremented by 2", pre.coins + 2, state.coins);
        assertEqual("Cards not drawn", pre.handCount[currentPlayer], state.handCount[currentPlayer]);
    }

}

void bug10(int cardChoice, int numToReturn, char* cardName)
{
    printTestName("Bug 10", "[Ambassador Card] The number of cards in the player's hand "
                            "eligible to be returned to the Supply should be correctly counted");
    printf("\t   Selected card: %s,\tNum to return: %d\n", cardName, numToReturn);

    // choice 1 is the hand position of the card to return to the supply
    // choice 2 is the number of that card to return to the supply
    // handPos is the position of the ambassador card itself

    // arrange
    struct gameState state;
    int k[10] = {adventurer, council_room, feast, gardens, mine,    //Set card array - include ambassador
                 remodel, smithy, village, ambassador, minion};
    int currentPlayer = 1;
    int nextPlayer = 0;
    int randomSeed = rand();
    initializeGame(NUM_PLAYERS, k, randomSeed, &state);

    // *********************************
    // Play the ambassador card

    // arrange
    state.whoseTurn = currentPlayer;

    // arrange current player's hand
    state.handCount[currentPlayer] = 1;
    state.hand[currentPlayer][0] = ambassador;                      //Set ambassador card

    if (numToReturn == 0) {                                         //Ensure have return card in hand
        state.handCount[currentPlayer]++;
        state.hand[currentPlayer][1] = cardChoice;
    } else if (numToReturn > 0) {                                     //Fill rest of hand with return card
        for (int i = 0; i < numToReturn; i++) {
            state.handCount[currentPlayer]++;
            state.hand[currentPlayer][i + 1] = cardChoice;
        }
    }

    // arrange opponent's discard
    state.discardCount[nextPlayer] = 0;

    // set supply of selected card to check player's card(s) returned to Supply
    state.supplyCount[cardChoice] = 0;

    // save current variable states
    struct gameState pre;
    memcpy(&pre, &state, sizeof(struct gameState));

    // check player's hand
//    for (int i = 0; i < state.handCount[currentPlayer]; i++) {
//        printf("Hand[%d]: %d\n", i, state.hand[currentPlayer][i]);
//    }

    // act
    // cardEffect(int card, int choice1, int choice2, int choice3, struct gameState *state, int handPos, int *bonus)
    // choice 1 is the hand position of the card to return to the supply
    // choice 2 is the number of that card to return to the supply
    int returnPosition = 1;
    int expectedCount = 0;
    int foundCard = 0;                      // use as bool to check for card in hand or supply
    int ret = cardEffect(ambassador, returnPosition, numToReturn, 0, &state, 0, 0);

    // tests for card choice is Ambassador
    if (cardChoice == ambassador) {
        assertEqual("cardEffect returned error (-1) with ambassador as card choice", ret, -1);
        return;
    }

    // tests for invalid number to return
    if (numToReturn < 0 || numToReturn > 2) {
        assertEqual("cardEffect returned error (-1) with invalid number to return", ret, -1);
        return;
    }

    // tests for choice to return 0, 1, or 2 cards
    if (numToReturn >= 0 && numToReturn <= 2) {

        // asserts
        assertEqual("cardEffect returned successfully after played ambassador", ret, 0);

        //check hand count and hand
        expectedCount = pre.handCount[currentPlayer] - numToReturn - 1;         // -1 for discarded ambassador
        assertEqual("Number of card choice decremented from player's hand",
                    expectedCount, state.handCount[currentPlayer]);

        if (numToReturn> 0) {
            foundCard = 0;
            for (int i = 0; i < state.handCount[currentPlayer]; i++) {
                if (state.hand[currentPlayer][i] == cardChoice) { foundCard = 1; }
            }
            assertEqual("Card to return not found in player's hand", foundCard, 0);
        }

        //check supply count
        if (numToReturn == 0) { expectedCount = pre.supplyCount[cardChoice]; }
        if (numToReturn >= 1) {
            expectedCount = pre.supplyCount[cardChoice] + numToReturn - (NUM_PLAYERS-1);
            if (expectedCount < 0) { expectedCount = 0; }
        }
        assertEqual("Correct supply count of card choice",
                    expectedCount, state.supplyCount[cardChoice]);

        //check each opponent's discard
        for (int i = 0; i < (NUM_PLAYERS-1); i++) {
            if (i != currentPlayer && state.supplyCount[cardChoice] > 0) {
                expectedCount = pre.discardCount[i] + 1;
                assertEqual("Opponent's discard count incremented correctly",
                            expectedCount, state.discardCount[nextPlayer]);
            }

        }

        foundCard = 0;
        for (int i = 0; i < state.discardCount[nextPlayer]; i++) {
            if (state.discard[nextPlayer][i] == cardChoice) { foundCard += 1; }
        }
        if (numToReturn == 0) { assertEqual("Card to return not found in opponent's discard", foundCard, 0); }
        if (numToReturn >= 1) { assertEqual("Card to return found in opponent's discard", foundCard, 1); }
    }
}


int main() {

    srand(time(NULL));

    // Test feast card with cardToGain of cost < 5, == 5, and > 5
    bug6(1, smithy,   "Smithy");         // supply count of 1, smithy has a cost of 4
    bug6(1, tribute,  "Tribute");        // tribute has a cost of 5
    bug6(1, province, "Province");       // province has a cost of 8 - infinite loop until dominion.c fixed
    bug6(0, smithy, "Smithy");           // test card not available in supply - infinite loop until dominion.c fixed
    bug6(1, -5, "Invalid Card");         // test invalid card to gain - infinite loop until dominion.c fixed

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

    //Test Ambassador
    // Tests for valid choices
    bug10(estate, 0, "Estate");          // return 0 cards
    bug10(duchy, 1, "Duchy");            // return 1 card
    bug10(adventurer, 2, "Adventurer");  // return 2 cards

    // Tests for invalid choices
    bug10(tribute, -1, "Tribute");       // return -1 cards (should return error)
    bug10(mine, 3, "Mine");              // return 3 cards (should return error)
    bug10(ambassador, 2, "Ambassador");  // return 2 ambassador cards (should return error)

    printf("\n[unittest_tayloreb] Tests complete\n");
    return 0;
}
