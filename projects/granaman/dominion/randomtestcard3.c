#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "my_utils.h"
#include "dominion.h"
#include <time.h>
#include <string.h>

// ************************
// Random tests for Tribute card

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state

// cardTribute params: int currentPlayer, struct gameState *state

void randomTestCard3() {

    int iteration = 0;

    while(1) {
        // arrange
        struct gameState state;
        int k[10] = {1,2,3,4,5,6,7,8,9,10};
        int currentPlayer = 1;
        int numPlayers = getRandomNumberOfPlayers();

        initializeGame(numPlayers, k, 2, &state);

        state.coins = rand() % 101; // starting coins between 0 and 100
        int choice = rand() % 2+1; // random between 0-1, shifted to 1-2

        // randomize each player's hand count and cards, discard count and cards
        int randomHandCount;
        int randomDeckCount;
        int randomDiscardCount;
        for (int i = 1; i < numPlayers; i++) {
            randomHandCount = randomizeHand(&state, i);
            randomDeckCount = randomizeDeck(&state, i);
            randomDiscardCount = randomizeDiscard(&state, i);
        }

        // randomize the position of the tribute card in the player's hand
        int randomHandPos = insertCardIntoHandAtRandomPosition(&state, currentPlayer, randomHandCount, tribute);

        int discardCountBefore = state.discardCount[currentPlayer];
        int coinsBefore = state.coins;
        int numActionsBefore = state.numActions;
        // record player hand sizes
        int playerHandCountBefore[4] = {0,0,0,0};
        int playerDeckCountBefore[4] = {0,0,0,0};
        int playerDiscardCountBefore[4] = {0,0,0,0};
        for (int i = 0; i < numPlayers; i++) {
            playerHandCountBefore[i] = state.handCount[i];
            playerDeckCountBefore[i] = state.deckCount[i];
            playerDiscardCountBefore[i] = state.discardCount[i];
        }

        // act
        printf("Tribute Random Test - Iteration #%d, # players: %d, hand count: %d, discard count: %d \n", iteration, numPlayers, randomHandCount, discardCountBefore);
        cardTribute(currentPlayer, &state);

        // assert
        int pLeft = currentPlayer +1;  // figure out who player to the left was
        if (pLeft > numPlayers-1) {
            pLeft = 0;
        }

        // figure out how many cards the player to the left had to reveal
        int opponentCardCount = playerDeckCountBefore[pLeft] + playerDiscardCountBefore[pLeft];

        if (opponentCardCount >= 2) {
            // opponent had 2 or more cards
            // no matter where they came from, they ultimately decreased the deck count by 2 and increased the discard pile by 2

            // note most of these will fail in the default implementation of dominion.c
            // because the cardTribute method does not properly discard revealed cards
            assertEqual("Opponent revealed two cards, opponent discard pile increased by 2", playerDiscardCountBefore[pLeft]+2, state.discardCount[pLeft]);
            int card1 = state.discard[pLeft][state.discardCount[pLeft]-1];
            int card2 = state.discard[pLeft][state.discardCount[pLeft]-2];
            printf("Opponent revealed these two cards: %d and %d\n", card1, card2);
        } else if (opponentCardCount == 1) {
            // opponent only had one card (in deck or in discard pile)
            // that card was revealed and discarded
        } else if (opponentCardCount == 0) {
            // opponent had no cards to reveal
            // player gains no reward of any kind
            assertEqual("Opponent revealed no cards, player gained no actions", numActionsBefore, state.numActions);
            assertEqual("Opponent revealed no cards, Player gained no coins", coinsBefore, state.coins);
        }

        iteration++;

        if (iteration == MAX_ITERATIONS) {
            printf("Done with %d iterations\n", MAX_ITERATIONS);
            break;
        }
    }
}

int main() {
    srand(time(NULL));
    randomTestCard3();
    return 0;
}
