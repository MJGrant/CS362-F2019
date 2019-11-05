#ifndef _MY_UTILS_H
#define _MY_UTILS_H

#include "dominion.h"

#define MAX_ITERATIONS 500
#define MAX_HAND_SIZE 10 // up to 500 is allowed but realistically the player's hand size is rarely over 10

int getRandomHandCount();
int getRandomDiscardCount();
int getRandomCard();
void setRandomHand(struct gameState *state, int currentPlayer, int randomHandCount);
void setRandomDiscardPile(struct gameState *state, int currentPlayer, int randomDiscardCount);
void setRandomEstateQuantity(struct gameState *state);
int getRandomNumberOfPlayers();
int insertCardIntoHandAtRandomPosition(struct gameState *state, int currentPlayer, int handCount, int card);
int randomizeHand(struct gameState *state, int player);
int randomizeDiscard(struct gameState *state, int player);

#endif
