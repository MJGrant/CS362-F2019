#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "my_asserts.h"
#include "dominion.h"
#include "dominion_helpers.h"
#include <time.h>

#define NUM_PLAYERS 2


// ************************
// Unit tests for bugs 2, 3, 4, 8

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state
// cardEffect(int card, int choice1, int choice2, int choice3, struct gameState *state, int handPos, int *bonus)


void bug2() {
	printTestName("Bug 2", "[Mine switch]  in cardEffect function, checks cost of card to trash vs cost of card to buy incorrectly");


}


void bug3() {
	printTestName("Bug 3", "[remodel case] in cardEffect function, the if statement that compares the two choice statements needs to be switched");

}

void bug4() {
	printTestName("Bug 4", "[isGameOver function]  While checking if there are 3 cards with a card count of 0, it only loops through 25 cards");

}


void bug8() {
	 printTestName("Bug 8", "[cardEffect] The number of bonus coins from actions does not appear to be recorded correctly");


}



int main() {

	srand(time(NULL));

    bug2();
    bug4();
    bug6();
    bug8();
    
    printf("[unittest_barwickl] Test complete\n");
    return 0;
}


