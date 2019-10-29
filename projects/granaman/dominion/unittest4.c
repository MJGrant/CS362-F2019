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

    printTestName("Tribute Card", "Player plays the Tribute card against a variety of opponent decks");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int opponent = 0;
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);

    int deckCountBefore, coinsBefore, numActionsBefore, handCountBefore;

    // *********************************
    // opponent's deck top 2: all action cards
    state.deckCount[opponent] = 2;
    state.deck[opponent][0] = mine;
    state.deck[opponent][1] = adventurer;

    numActionsBefore = state.numActions;

    // act
    cardTribute(1, &state);

    // assert two actions were gained
    assertEqual("Opponent revealed 2 action cards, player gained +2 actions", numActionsBefore+2, state.numActions);


    // *********************************
    // opponent's deck top 2: all treasure cards
    state.deckCount[opponent] = 2;
    state.deck[opponent][0] = copper;
    state.deck[opponent][1] = silver;

    coinsBefore = state.coins;

    // act
    cardTribute(1, &state);

    // assert two coins were gained
    assertEqual("Opponent revealed 2 treasure cards, player gained +2 coins", coinsBefore+2, state.coins);

    // *********************************
    // opponent's deck top 2: all victory cards
    state.deckCount[opponent] = 2;
    state.deck[opponent][0] = estate;
    state.deck[opponent][1] = duchy;

    deckCountBefore = state.deckCount[currentPlayer];
    handCountBefore = state.handCount[currentPlayer];

    // act
    cardTribute(1, &state);

    // assert two cards were drawn from player's own deck
    assertEqual("Opponent revealed 2 victory cards, player drew +2 cards from their own deck", deckCountBefore+2, state.deckCount[currentPlayer]);
    assertEqual("Player's deck decreased in size by 2", deckCountBefore-2, state.deckCount[currentPlayer]);
    assertEqual("Player's hand size increased by 2", handCountBefore+2, state.handCount[currentPlayer]);


    // *********************************
    // opponent's deck top 2: all curse cards
    state.deckCount[opponent] = 2;
    state.deck[opponent][0] = curse;
    state.deck[opponent][1] = curse;

    numActionsBefore = state.numActions;
    coinsBefore = state.coins;
    deckCountBefore = state.deckCount[currentPlayer];

    // act
    cardTribute(1, &state);

    // assert nothing was gained
    // first one fails because curse cards are incorrectly treated as actions in the code
    assertEqual("[EXISTING BUG] Opponent revealed 2 curse cards, player gained no actions", numActionsBefore, state.numActions);
    assertEqual("Opponent revealed 2 curse cards, player gained no coins", coinsBefore, state.coins);
    assertEqual("Opponent revealed 2 curse cards, player drew no cards", deckCountBefore, state.deckCount[currentPlayer]);


    // *********************************
    // opponent's deck top 2: one action card and one treasure card
    state.deckCount[opponent] = 2;
    state.deck[opponent][0] = mine;
    state.deck[opponent][1] = copper;

    numActionsBefore = state.numActions;
    coinsBefore = state.coins;

    // act
    cardTribute(1, &state);

    // assert two actions were gained and two coins were gained
    assertEqual("Opponent revealed 1 action card, player gained +2 actions", numActionsBefore+2, state.numActions);
    assertEqual("Opponent revealed 1 treasure card, player gained +2 coins", coinsBefore+2, state.coins);


    // *********************************
    // opponent's deck top 2: one action card and one victory card
    state.deckCount[opponent] = 2;
    state.deck[opponent][0] = ambassador;
    state.deck[opponent][1] = duchy;

    numActionsBefore = state.numActions;
    deckCountBefore = state.deckCount[currentPlayer];
    handCountBefore = state.handCount[currentPlayer];

    // act
    cardTribute(1, &state);

    // assert two actions were gained and two coins were gained
    assertEqual("Opponent revealed 1 action card, player gained +2 actions", numActionsBefore+2, state.numActions);
    assertEqual("Opponent revealed 1 victory card, player drew +2 cards to their own deck", deckCountBefore+2, state.deckCount[currentPlayer]);
    assertEqual("Player's deck decreased in size by 2", deckCountBefore-2, state.deckCount[currentPlayer]);
    assertEqual("Player's hand size increased by 2", handCountBefore+2, state.handCount[currentPlayer]);


    // *********************************
    // opponent's deck top 2: one action card and one curse card
    state.deckCount[opponent] = 2;
    state.deck[opponent][0] = ambassador;
    state.deck[opponent][1] = duchy;

    numActionsBefore = state.numActions;
    deckCountBefore = state.deckCount[currentPlayer];
    handCountBefore = state.handCount[currentPlayer];

    // act
    cardTribute(1, &state);

    // assert two actions were gained from the action card but nothing changed from the curse card
    assertEqual("Opponent revealed 1 action card, player gained +2 actions", numActionsBefore+2, state.numActions);
    assertEqual("Opponent revealed 1 curse card, player gained no coins", coinsBefore, state.coins);
    assertEqual("Opponent revealed 1 curse card, player drew no cards", deckCountBefore, state.deckCount[currentPlayer]);


    // *********************************
    // opponent's deck top 2: one treasure card and one victory card
    state.deckCount[opponent] = 2;
    state.deck[opponent][0] = copper;
    state.deck[opponent][1] = estate;

    coinsBefore = state.coins;
    deckCountBefore = state.deckCount[currentPlayer];
    handCountBefore = state.handCount[currentPlayer];

    // act
    cardTribute(1, &state);

    // assert two coins were gained from the treasure card and 2 cards were drawn for the victory card
    assertEqual("Opponent revealed 1 treasure cards, player gained +2 coins", coinsBefore+2, state.coins);
    assertEqual("Opponent revealed 1 victory card, player drew +2 cards from their own deck", deckCountBefore+2, state.deckCount[currentPlayer]);
    assertEqual("Player's deck decreased in size by 2", deckCountBefore-2, state.deckCount[currentPlayer]);
    assertEqual("Player's hand size increased by 2", handCountBefore+2, state.handCount[currentPlayer]);


    // *********************************
    // opponent's deck top 2: one treasure card and one curse card
    state.deckCount[opponent] = 2;
    state.deck[opponent][0] = copper;
    state.deck[opponent][1] = curse;

    coinsBefore = state.coins;
    deckCountBefore = state.deckCount[currentPlayer];
    handCountBefore = state.handCount[currentPlayer];

    // act
    cardTribute(1, &state);

    // assert two cards were gained from the victory card and nothing else changed from the curse card
    assertEqual("Opponent revealed 1 treasure card, player gained +2 coins", coinsBefore+2, state.coins);
    assertEqual("Opponent revealed 1 curse card, player gained no actions", numActionsBefore, state.numActions);
    assertEqual("Opponent revealed 1 curse card, player drew no cards", deckCountBefore, state.deckCount[currentPlayer]);

    // *********************************
    // opponent's deck top 2: one victory card and one curse card
    state.deckCount[opponent] = 2;
    state.deck[opponent][0] = duchy;
    state.deck[opponent][1] = curse;

    coinsBefore = state.coins;
    deckCountBefore = state.deckCount[currentPlayer];
    handCountBefore = state.handCount[currentPlayer];

    // act
    cardTribute(1, &state);

    // assert two cards were gained from the victory card and nothing else changed from the curse card
    assertEqual("Opponent revealed 1 victory card, player drew +2 cards from their own deck", deckCountBefore+2, state.deckCount[currentPlayer]);
    assertEqual("Player's deck decreased in size by 2", deckCountBefore-2, state.deckCount[currentPlayer]);
    assertEqual("Player's hand size increased by 2", handCountBefore+2, state.handCount[currentPlayer]);
    assertEqual("Opponent revealed 1 curse card, player gained no actions", numActionsBefore, state.numActions);
    assertEqual("Opponent revealed 1 curse card, player gained no coins", coinsBefore, state.coins);
}

void tributeTest2() {
    printTestName("Tribute Card", "Opponent deck/discard pile edge cases");

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int opponent = 0;
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);

    int deckCountBefore, coinsBefore, numActionsBefore, handCountBefore;

    // *********************************
    // opponent's deck only has 1 card in it
    // opponent's discard pile has a card to draw to fill in
    printTestName("Tribute Card", "Opponent has 1 card in deck, 1 in discard pile");
    state.deckCount[opponent] = 1;
    state.deck[opponent][0] = estate;

    state.discardCount[opponent] = 1;
    state.discard[opponent][0] = copper;

    // this particular set of cards is a victory card and a treasure card
    // expect to get +2 cards drawn and +2 coins for having them revealed

    numActionsBefore = state.numActions;
    coinsBefore = state.coins;
    deckCountBefore = state.deckCount[currentPlayer];
    handCountBefore = state.handCount[currentPlayer];

    // act
    cardTribute(1, &state);

    // assert two cards were gained from the victory card and nothing else changed from the curse card
    assertEqual("Opponent revealed 1 victory card, player drew +2 cards from their own deck", deckCountBefore+2, state.deckCount[currentPlayer]);
    assertEqual("Player's deck decreased in size by 2", deckCountBefore-2, state.deckCount[currentPlayer]);
    assertEqual("Player's hand size increased by 2", handCountBefore+2, state.handCount[currentPlayer]);
    assertEqual("Opponent revealed 1 treasure card, player gained +2 coins", coinsBefore+2, state.coins);
    assertEqual("Player gained no actions", numActionsBefore, state.numActions);


    // *********************************
    // opponent's deck is empty
    // opponent's discard pile has 2 cards in it to draw from
    printTestName("Tribute Card", "Opponent has no cards in deck, two in discard pile");
    state.deckCount[opponent] = 0;

    state.discardCount[opponent] = 2;
    state.discard[opponent][0] = copper;
    state.discard[opponent][1] = estate;

    // this particular set of cards is a victory card and a treasure card
    // expect to get +2 cards drawn and +2 coins for having them revealed

    numActionsBefore = state.numActions;
    coinsBefore = state.coins;
    deckCountBefore = state.deckCount[currentPlayer];
    handCountBefore = state.handCount[currentPlayer];

    // act
    cardTribute(1, &state);

    // assert two cards were gained from the victory card and nothing else changed from the curse card
    assertEqual("Opponent revealed 1 victory card, player drew +2 cards from their own deck", deckCountBefore+2, state.deckCount[currentPlayer]);
    assertEqual("Player's deck decreased in size by 2", deckCountBefore-2, state.deckCount[currentPlayer]);
    assertEqual("Player's hand size increased by 2", handCountBefore+2, state.handCount[currentPlayer]);
    assertEqual("Opponent revealed 1 treasure card, player gained +2 coins", coinsBefore+2, state.coins);
    assertEqual("Player gained no actions", numActionsBefore, state.numActions);


    // *********************************
    // opponent's deck and discard piles are empty
    // opponent's discard pile has 2 cards in it to draw from
    printTestName("Tribute Card", "Opponent has no cards in deck and no cards in discard pile");
    state.deckCount[opponent] = 0;

    state.discardCount[opponent] = 0;

    // player gets nothing from this impoverished opponent

    numActionsBefore = state.numActions;
    coinsBefore = state.coins;
    deckCountBefore = state.deckCount[currentPlayer];
    handCountBefore = state.handCount[currentPlayer];

    // act
    cardTribute(1, &state);

    // assert two cards were gained from the victory card and nothing else changed from the curse card
    assertEqual("Opponent has no cards, player gained no actions", numActionsBefore, state.numActions);
    assertEqual("Opponent has no cards, player's deck remains unchanged", deckCountBefore, state.deckCount[currentPlayer]);
    assertEqual("Opponent has no cards, player gains no coins", coinsBefore, state.coins);

}

int main() {
    tributeTest1();
    tributeTest2();

    printf("[Tribute Card test] Test complete\n");
    return 0;
}
