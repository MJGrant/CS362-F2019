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
    // Play the feast card with a valid card

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


int main() {

    srand(time(NULL));

    // Test feast card with cardToGain of cost < 5, == 5, and > 5
    bug6(1, smithy,   "Smithy");         // supply count of 1, smithy has a cost of 4
    bug6(1, tribute,  "Tribute");        // tribute has a cost of 5
    bug6(1, province, "Province");       // province has a cost of 8

    //Test feast card with card not available in supply
    bug6(0, smithy, "Smithy");

    //Test feast card with invalid card to gain
    bug6(1, -5, "Invalid Card");

    printf("\n[unittest_tayloreb] Test complete\n");
    return 0;
}
