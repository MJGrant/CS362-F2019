#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "dominion.h"
#include <time.h>

// ************************
// Unit tests for Tribute card

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state
// cardTribute params: int currentPlayer, struct gameState *state

void tributeTest1() {
    // player picks a card from their hand that is not the ambassador card itself
    // and the player picks a valid quantity of that card to discard (1 or 2)
    // this is the "intended" happy path of gameplay for this card

    printTestName("Tribute Card", "Player plays the Tribute card successfully as-intended");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int opponent = 0;
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);

    // *********************************
    // opponent's deck top 2: all action cards
    state.deckCount[opponent] = 2;
    state.deck[opponent][0] = mine;
    state.deck[opponent][1] = adventurer;

    int numActionsBefore = state.numActions;

    // act
    cardTribute(1, &state);

    // assert two actions were gained
    assertEqual("Opponent revealed 2 action cards, player gained +2 actions", numActionsBefore+2, state.numActions);


    // *********************************
    // opponent's deck top 2: all treasure cards
    state.deckCount[opponent] = 2;
    state.deck[opponent][0] = copper;
    state.deck[opponent][1] = silver;

    int coinsBefore = state.coins;

    // act
    cardTribute(1, &state);

    // assert two coins were gained
    assertEqual("Opponent revealed 2 treasure cards, player gained +2 coins", coinsBefore+2, state.coins);

    // *********************************
    // opponent's deck top 2: all victory cards
    state.deckCount[opponent] = 2;
    state.deck[opponent][0] = estate;
    state.deck[opponent][1] = duchy;

    int deckCountBefore = state.deckCount[currentPlayer];

    // act
    cardTribute(1, &state);

    // assert two cards were drawn from player's own deck
    assertEqual("Opponent revealed 2 victory cards, player drew +2 cards from their own deck", deckCountBefore+2, state.deckCount[currentPlayer]);
    assertEqual("Player's deck decreased in size by 2", deckCountBefore-2, state.deckCount[currentPlayer]);



}

int main() {
    tributeTest1();

    printf("[Tribute Card test] Test complete\n");
    return 0;
}
