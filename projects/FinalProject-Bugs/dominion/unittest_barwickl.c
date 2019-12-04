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

    // arrange
    int k[10] = {adventurer, council_room, feast, gardens, mine, 
                 remodel, ambassador, tribute, baron, minion};
    
    struct gameState G;
    int randomSeed = rand();
    int player1 = G.whoseTurn;
    int coinBonus;
    
    printf("    ------ TEST 1 ------\n");
    
    memset(&G, 23, sizeof(struct gameState)); 

    // initialize new game
    initializeGame(NUM_PLAYERS, k, randomSeed, &G); 

    // set up player's hand
    G.handCount[player1] = 2;
    G.hand[player1][0] = mine; 
    G.hand[player1][1] = silver;

    // act - call cardeffect function
    cardEffect(mine, 1, gold, 0, &G, 0, &coinBonus);

    int testnum1 = G.hand[player1][1];

    // assert - gained card is gold
    assertEqual("Player should be able to trash Silver to gain Gold", testnum1, gold);

    printf("\n");

    printf("    ------ TEST 2 ------\n");

    memset(&G, 23, sizeof(struct gameState)); 

    // initialize new game
    initializeGame(NUM_PLAYERS, k, randomSeed, &G); 

    // set up player's hand
    G.handCount[player1] = 2;
    G.hand[player1][0] = mine; 
    G.hand[player1][1] = copper;

    // act - call cardEffect function
    cardEffect(mine, 1, silver, 0, &G, 0, &coinBonus);

    int testnum2 = G.hand[player1][1];
    
    // assert - gained card is silver
    assertEqual("Player should be able to trash Copper to gain Silver", testnum2, silver);

    printf("\n");
    
}


void bug3() {
    printTestName("Bug 3", "[remodel case] in cardEffect function, the if statement that compares the two choice statements needs to be switched");

    // arrange
    int k[10] = {adventurer, council_room, feast, gardens, mine, 
                 remodel, ambassador, tribute, baron, minion};
   
    struct gameState G;
    int randomSeed = rand();
    int player1 = G.whoseTurn;
    int coinBonus;
 
    printf("    ------ TEST 1 ------\n");
    
    memset(&G, 23, sizeof(struct gameState));

    // initialize new game 
    initializeGame(NUM_PLAYERS, k, randomSeed, &G); 

    // set up player's hand
    G.handCount[player1] = 2;
    G.hand[player1][0] = remodel;
    G.hand[player1][1] = duchy;
    
    
    // set player discard pile to empty
    G.discardCount[player1] = 0;
    G.discard[player1][1] = -1;
    G.discard[player1][0] = -1;
    
    // act
    int ret = cardEffect(remodel, 1, gold, 0, &G, 0, &coinBonus);

    // assert - no error should be returned when cost is not too expensive    
    assertEqual("Trash Duchy to gain Gold test", ret, 0);

    // discard count should not be empty
    int dicardPileCount1 = G.discardCount[0];
    assertEqual("Discard count should have one card test", dicardPileCount1, 1);

    printf("\n");

}

void bug4() {
    printTestName("Bug 4", "[isGameOver function]  While checking if there are 3 cards with a card count of 0, it only loops through 25 cards");

    // arrange
    int k[10] = {adventurer, council_room, sea_hag, gardens, mine, 
                 remodel, ambassador, tribute, baron, minion};
        
    struct gameState G;
    int randomSeed = rand();
   
    printf("    ------ TEST 1 ------\n");
    
    memset(&G, 23, sizeof(struct gameState)); 
    
    // initialize new game
    initializeGame(NUM_PLAYERS, k, randomSeed, &G); 

    int begin_isGameOver = isGameOver(&G);
    // check that game has not ended
    assertEqual("Game NOT over at beginning, prior to setting three empty supply piles", begin_isGameOver, 0);

    // act - set three card supply piles to be empty
    G.supplyCount[council_room] = 0;
    G.supplyCount[sea_hag] = 0;
    G.supplyCount[mine] = 0;

    // check final game status
    int end_isGameOver = isGameOver(&G);

    // assert - game should be over after setting three supply piles to empty
    assertEqual("Game should be over once threre are three empty supply piles", end_isGameOver, 1);
   
    printf("\n");
   
}


void bug8() {
    printTestName("Bug 8", "[cardEffect] The number of bonus coins from actions does not appear to be recorded correctly");

    // arrrange 
    int k[10] = {adventurer, council_room, feast, gardens, mine,
                 remodel, smithy, village, baron, great_hall};
    
    // Setup Game State
    struct gameState G;
    int randomSeed = rand();
    int player1 = G.whoseTurn;

    printf("    ------ TEST 1 ------\n");

    memset(&G, 23, sizeof(struct gameState));

    // initialize new game
    initializeGame(NUM_PLAYERS, k, randomSeed, &G);

    // reset player hand
    for (int i = 0; i < G.handCount[player1]; i++)
    {
        G.hand[player1][i] = -1;
    }
    G.handCount[player1] = 0;

    // set cards in player's hand 
    G.hand[player1][0] = baron;
    G.hand[player1][1] = estate;
    G.handCount[player1] = 2;

    // update coins to account for potential impact to changing first card
    updateCoins(0, &G, 0);

    // save current game state 
    struct gameState preG;
    memcpy(&preG, &G, sizeof(struct gameState));

    // act - call playCard function
    playCard(0, 1, 0, 0, &G);

    // assert - End of game coins should include coins gained during play 
    assertEqual("End of game coins should equal beginning of game +4 gained test", (preG.coins + 4), G.coins );
    
    printf("\n");

    printf("    ------ TEST 2 ------\n");

    memset(&G, 23, sizeof(struct gameState)); 

    initializeGame(NUM_PLAYERS, k, randomSeed, &G);

    // rest player hand
    for (int i = 0; i < G.handCount[player1]; i++)
    {
        G.hand[player1][i] = -1;
    }
    G.handCount[player1] = 0;

    // set cards in player's hand 
    G.hand[player1][0] = minion;
    G.hand[player1][1] = estate;
    G.handCount[player1] = 2;

    // update coins to account for potential impact to changing first card
    updateCoins(0, &G, 0);

    // save current game state 
    struct gameState preG2;
    memcpy(&preG2, &G, sizeof(struct gameState));

    // call playCard function
    playCard(0, 1, 0, 0, &G);

    // assert - End of game coins should include coins gained during play 
    assertEqual("End of game coins should equal beginning of game +2 gained test", (preG2.coins + 2), G.coins );

    printf("\n");
    
}


int main() {

    srand(time(NULL));

    bug2();
    bug3();
    bug4();
    bug8();

        
    printf("[unittest_barwickl] Test complete\n");
    return 0;
}


