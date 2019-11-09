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

// helper method for determining what type of card a given card is
int getCardType(int card) {
    int ret = -1;
    if (card == copper || card == silver || card == gold) {
        // treasure card
        ret = 1;
    } else if (card == estate || card == duchy || card == province || card == great_hall || card == gardens) {
        // victory card
        ret = 2;
    } else if (card == adventurer || card == council_room || card == feast || card == mine || card == remodel || card == smithy || card == village || card == baron || card == minion || card == steward || card == tribute || card == ambassador || card == cutpurse || card == embargo || card == outpost || card == salvager || card == sea_hag || card == treasure_map) {
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

void randomTestCard3() {

    int iteration = 0;

    // use to track that we've encountered each individual card in each possible position
    int cardTracker1 = 0;
    int cardTracker2 = 0;

    int everyCombination = 0;
    int everyCardTypeAlone = 0;

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
        for (int i = 0; i < numPlayers; i++) {
            randomHandCount = randomizeHand(&state, i)+1;
            randomDeckCount = randomizeDeck(&state, i)+1;
            randomDiscardCount = randomizeDiscard(&state, i)+1;
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

        int pLeft = currentPlayer +1;  // figure out who opponent (player to the left) is
        if (pLeft > numPlayers-1) {
            pLeft = 0;
        }

        // record how many cards the opponent has total between deck and discard piles
        int opponentCardCount = playerDeckCountBefore[pLeft] + playerDiscardCountBefore[pLeft];

        printf("Tribute Random Test - Iteration #%d, # players: %d, hand count: %d, discard count: %d \n", iteration, numPlayers, randomHandCount, discardCountBefore);
        printf("Opponent has %d cards in hand, %d cards in deck, and %d cards in discard pile\n", state.handCount[pLeft], state.deckCount[pLeft], state.discardCount[pLeft]);

        // act
        cardTribute(currentPlayer, &state);

        // assert
        if (opponentCardCount >= 2) {
            // opponent had 2 or more cards
            // no matter where they came from, they ultimately decreased the deck count by 2 and increased the discard pile by 2

            // note most of these will fail in the default implementation of dominion.c
            // because the cardTribute method does not properly discard revealed cards

            assertEqual("Opponent revealed two cards, opponent discard pile increased by 2", playerDiscardCountBefore[pLeft]+2, state.discardCount[pLeft]);
            int card1 = state.discard[pLeft][state.discardCount[pLeft]-1];
            int card2 = state.discard[pLeft][state.discardCount[pLeft]-2];

            printf("Opponent [player %d] revealed these TWO cards: %d and %d\n", pLeft, card1, card2);

            // this is for (eventually) confirming that we drew every possible card at least once
            if (card1 == cardTracker1) {
                cardTracker1++;
            }

            if (card2 == cardTracker2) {
                cardTracker2++;
            }

            int cardType1 = getCardType(card1);
            int cardType2 = getCardType(card2);

            if (cardType1 == 1 && cardType2 == 1) {
                assertEqual("[Treasure Card & Treasure Card] player gained +2 coins", coinsBefore+2, state.coins);
                everyCombination++;
            } else if (cardType1 == 2 && cardType2 == 2) {
                assertEqual("[Victory Card & Victory Card] player drew +2 cards to hand", playerHandCountBefore[currentPlayer]+2, state.handCount[currentPlayer]);
                everyCombination++;
            } else if (cardType1 == 3 && cardType2 == 3) {
                assertEqual("[Action Card & Action Card] player gained +2 actions", numActionsBefore+2, state.numActions);
                everyCombination++;
            } else if (cardType1 == 4 && cardType2 == 4) {
                assertEqual("[Action-Victory Card & Action-Victory Card] player gained +2 actions", numActionsBefore+2, state.numActions);
                assertEqual("[Action-Victory Card & Action-Victory Card] player drew +2 cards to hand", playerHandCountBefore[currentPlayer]+2, state.handCount[currentPlayer]);
                everyCombination++;
            } else if (cardType1 == 5  && cardType2 == 5) {
                assertEqual("[Curse Card & Curse Card] player drew no cards to hand", playerHandCountBefore[currentPlayer], state.handCount[currentPlayer]);
                assertEqual("[Curse Card & Curse Card] player gained no actions", numActionsBefore, state.numActions);
                assertEqual("[Curse Card & Curse Card] player gained no coins", coinsBefore, state.coins);
                everyCombination++; // 5
            } else if ((cardType1 == 1 && cardType2 == 2 )|| (cardType1 == 2 && cardType2 == 1)) {
                assertEqual("[Treasure Card & Victory Card] player gained +2 coins", coinsBefore+2, state.coins);
                assertEqual("[Treasure Card & Victory Card] player drew +2 cards to hand", playerHandCountBefore[currentPlayer]+2, state.handCount[currentPlayer]);
                everyCombination++; // 6
            } else if ((cardType1 == 1 && cardType2 == 3) || (cardType1 == 3 && cardType2 == 1)) {
                assertEqual("[Treasure Card & Action Card] player gained +2 coins", coinsBefore+2, state.coins);
                assertEqual("[Treasure Card & Action Card] player gained 2 actions", numActionsBefore+2, state.numActions);
                everyCombination++; // 7
            } else if ((cardType1 == 1 && cardType2 == 4) || (cardType1 == 4 && cardType2 == 1)) {
                assertEqual("[Treasure Card & Action-Victory Card] player gained +2 coins", coinsBefore+2, state.coins);
                // 4 is "action-victory" dual-type card
                assertEqual("[Treasure Card & Action-Victory Card] player gained +2 actions", numActionsBefore+2, state.numActions);
                assertEqual("[Treasure Card & Action-Victory Card] player drew +2 cards to hand", playerHandCountBefore[currentPlayer]+2, state.handCount[currentPlayer]);
                everyCombination++; // 8
            } else if ((cardType1 == 1 && cardType2 == 5) || (cardType1 == 5 && cardType2 == 1)) {
                assertEqual("[Treasure Card & Curse Card] player gained +2 coins", coinsBefore+2, state.coins);
                assertEqual("[Treasure Card & Curse Card] player drew no cards to hand", playerHandCountBefore[currentPlayer], state.handCount[currentPlayer]);
                assertEqual("[Treasure Card & Curse Card] player gained no actions", numActionsBefore, state.numActions);
                everyCombination++; // 9
            } else if ((cardType1 == 2 && cardType2 == 3) || (cardType1 == 3 && cardType2 == 2)) {
                assertEqual("[Action Card & Victory Card] player gained +2 actions", numActionsBefore+2, state.numActions);
                assertEqual("[Action Card & Victory Card] player drew +2 cards to hand", playerHandCountBefore[currentPlayer]+2, state.handCount[currentPlayer]);
                everyCombination++; // 10
            } else if ((cardType1 == 2 && cardType2 == 4) || (cardType1 == 4 && cardType2 == 2)) {
                assertEqual("[Victory Card & Action-Victory Card] player gained +4 actions", numActionsBefore+4, state.numActions);
                assertEqual("[Victory Card & Action-Victory Card] player drew +2 cards to hand", playerHandCountBefore[currentPlayer]+2, state.handCount[currentPlayer]);
                everyCombination++; // 11
            } else if ((cardType1 == 2 && cardType2 == 5) || (cardType1 == 5 || cardType2 == 2)) {
                assertEqual("[Victory Card & Curse Card] player gained no actions", numActionsBefore, state.numActions);
                assertEqual("[Victory Card & Curse Card] player drew +2 cards to hand", playerHandCountBefore[currentPlayer]+2, state.handCount[currentPlayer]);
                assertEqual("[Victory Card & Curse Card] player gained no coins", coinsBefore, state.coins);
                everyCombination++; // 12
            } else if ((cardType1 == 3 && cardType2 == 4) || (cardType1 == 4 && cardType2 == 3)) {
                // 4 is "action-victory" dual-type card, combine with the action card for +4 actions
                assertEqual("[Action Card & Action-Victory Card] player gained +4 actions", numActionsBefore+4, state.numActions);
                assertEqual("[Action Card & Action-Victory Card] player drew +2 cards to hand", playerHandCountBefore[currentPlayer]+2, state.handCount[currentPlayer]);
                everyCombination++; // 13
            } else if ((cardType1 == 3 && cardType2 == 5) || (cardType1 == 5 && cardType2 == 3)) {
                assertEqual("[Action Card & Curse Card] player gained +2 actions", numActionsBefore+2, state.numActions);
                assertEqual("[Action Card & Curse Card] player drew no cards to hand", playerHandCountBefore[currentPlayer], state.handCount[currentPlayer]);
                assertEqual("[Action Card & Curse Card] player gained no coins", coinsBefore, state.coins);
                everyCombination++; // 14
            } else if ((cardType1 == 4 && cardType2 == 5) || (cardType1 == 5 && cardType2 == 4)) {
                assertEqual("[Action-Victory Card & Curse Card] player gained +2 actions", numActionsBefore+2, state.numActions);
                assertEqual("[Action-Victory Card & Curse Card] player drew +2 cards to hand", playerHandCountBefore[currentPlayer]+2, state.handCount[currentPlayer]);
                assertEqual("[Action-Victory Card & Curse Card] player gained no coins", coinsBefore, state.coins);
                everyCombination++; // 15
            } else {
                printf("1 or more cards is an unhandled type!\n");
            }
        } else if (opponentCardCount == 1) {
            // opponent only had one card (1 in deck or 1 in discard pile and 0 in the other)
            // that card was revealed and discarded
            int card1 = state.discard[pLeft][state.discardCount[pLeft]-1];

            if (card1 == cardTracker1) {
                cardTracker1++;
            }

            int cardType1 = getCardType(card1);
            if (cardType1 == 1) {
                assertEqual("[Treasure Card alone] player gained +2 coins", coinsBefore+2, state.coins);
                everyCardTypeAlone++;
            } else if (cardType1 == 2) {
                assertEqual("[Victory Card alone] player drew +2 cards to hand", playerHandCountBefore[currentPlayer]+2, state.handCount[currentPlayer]);
                everyCardTypeAlone++;
            } else if (cardType1 == 3) {
                assertEqual("[Action Card alone] player gained +2 actions", numActionsBefore+2, state.numActions);
                everyCardTypeAlone++;
            } else if (cardType1 == 4) {
                assertEqual("[Action-Victory Card alone] player gained 2 actions", numActionsBefore+2, state.numActions);
                assertEqual("[Action-Victory Card alone] player drew +2 cards to hand", playerHandCountBefore[currentPlayer]+2, state.handCount[currentPlayer]);
                everyCardTypeAlone++;
            } else if (cardType1 == 5) {
                assertEqual("[Curse Card alone] player drew no cards to hand", playerHandCountBefore[currentPlayer], state.handCount[currentPlayer]);
                assertEqual("[Curse Card alone] player gained no actions", numActionsBefore, state.numActions);
                assertEqual("[Curse Card alone] player gained no coins", coinsBefore, state.coins);
                everyCardTypeAlone++;
            } else {
                printf("Card is an unhandled type!\n");
            }

        } else if (opponentCardCount == 0) {
            printf("Opponent reveals NO CARDS!\n");
            // opponent had no cards to reveal, player gains no reward of any kind
            assertEqual("Opponent revealed no cards, player drew no cards to hand", playerHandCountBefore[currentPlayer], state.handCount[currentPlayer]);
            assertEqual("Opponent revealed no cards, player gained no actions", numActionsBefore, state.numActions);
            assertEqual("Opponent revealed no cards, player gained no coins", coinsBefore, state.coins);
        }

        iteration++;

        if (iteration == MAX_ITERATIONS) {
            assertEqual("Every possible card type was drawn alone at least once", everyCardTypeAlone, 5);
            assertEqual("Every possible combination of card types happened at least once", everyCombination, 15);
            assertEqual("Every possible card was drawn at least once for 'card 1'", cardTracker1, 26);
            assertEqual("Every possible card was drawn at least once for 'card 2'", cardTracker2, 26);
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
