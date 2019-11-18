#ifndef _MY_UTILS_H
#define _MY_UTILS_H

#include "dominion.h"

#define MAX_ITERATIONS 2000
#define MAX_HAND_SIZE 10 // up to 500 is allowed but realistically the player's hand size is rarely over 10
#define MAX_DECK_SIZE 4
#define MAX_DISCARD_SIZE 4

int getCardType(int card);
int getRandomHandCount();
int getRandomDeckCount();
int getRandomDiscardCount();
int getRandomCard();
void setRandomHand(struct gameState *state, int currentPlayer, int randomHandCount);
void setRandomDeck(struct gameState *state, int currentPlayer, int randomDeckCount);
void setRandomDiscardPile(struct gameState *state, int currentPlayer, int randomDiscardCount);
void setRandomEstateQuantity(struct gameState *state);
int getRandomNumberOfPlayers();
int insertCardIntoHandAtRandomPosition(struct gameState *state, int currentPlayer, int handCount, int card);
int randomizeHand(struct gameState *state, int player);
int randomizeDeck(struct gameState *state, int player);
int randomizeDiscard(struct gameState *state, int player);

#endif
