#include <stdlib.h>
#include <stdio.h>
#include "my_asserts.h"
#include "dominion.h"
#include <time.h>

// ************************
// Unit tests for Ambassador card

// initializeGame params: int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state

// OLD cardAmbassador params: int currentPlayer, int choice1, int choice2, struct gameState *state, int handPos)
// NEW ambassadorRefactor params: int choice1, int choice2, struct gameState *state, int handPos

//choice1 represents the card the player has chosen to discard
//choice2 represents how many of that card the player has chosen to discard

void ambassadorTest1() {
    // player picks a card from their hand that is not the ambassador card itself
    // and the player picks a valid quantity of that card to discard (1 or 2)
    // this is the "intended" happy path of gameplay for this card

    printTestName("Ambassador Card", "Player plays the Ambassador card successfully as-intended");
    // Note: the player is discarding 2 estate cards, but they are playing against 1 other player
    // in this case, the opponent gains +1 estate card and the extra estate card remains in the supply pile

    // arrange
    struct gameState state;
    int k[10] = {1,2,3,4,5,6,7,8,9,10};
    int currentPlayer = 1;
    initializeGame(2, k, 2, &state);
    int ret;

    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = ambassador;
    state.hand[currentPlayer][1] = minion;
    state.hand[currentPlayer][2] = estate;
    state.hand[currentPlayer][3] = estate;

    int estateSupplyBefore = state.supplyCount[estate];
    int opponentHandSizeBefore = state.handCount[0];

    // act
    // set choice1 (second param) to estate to indicate which card to reveal
    // set choice2 (param after that) to indicate how many
    // the last param is the position of the ambassador card itself in the player's hand
    ret = ambassadorRefactor(2, 2, &state, 0);

    // assert
    int ambassadorHandCount = 0;
    int estateHandCount = 0;
    for (int i = 0; i < state.handCount[currentPlayer]; i++) {
        if (state.hand[currentPlayer][i] == ambassador) {
            ambassadorHandCount++;
        }
        if (state.hand[currentPlayer][i] == estate) {
            estateHandCount++;
        }
    }

    assertEqual("The Ambassador card was removed from the player's hand", ambassadorHandCount, 0);
    // this fails because the ambassador method is trashing the card instead of discarding it
    assertEqual("[EXISTING BUG] Ambassador card added to player's discard pile", state.discard[currentPlayer][state.discardCount[currentPlayer]-1], estate);
    // this fails because the trash loop at the end of cardAmbassador should compare to choice1 not state->hand[currentPlayer][choice1]
    assertEqual("[EXISTING BUG] The player no longer has any estate cards in their hand", estateHandCount, 0);
    assertIncreasedByOne("The Estate supply pile has grown +1 in size", estateSupplyBefore, state.supplyCount[estate]);
    assertIncreasedByOne("[EXISTING BUG] The opponent hand size has increased by one", opponentHandSizeBefore, state.handCount[0]);
    assertEqual("[EXISTING BUG] The opponent's hand has gained an Estate card", state.hand[0][state.handCount[0]], estate);

    // test the error checks at the start of cardAmbassador

    // act
    // set choice1 and handpos to the same number (the ambassador card itself is at 0)
    ret = ambassadorRefactor(0, 1, &state, 0);

    // assert
    assertEqual("Attempting to discard the Ambassador card itself is rejected", ret, -1);

    // act
    // player attempts to discard more than the allowed number of cards
    ret = ambassadorRefactor(2, 3, &state, 0);

    // assert
    assertEqual("Attempting to discard 3 copies of a card is rejected", ret, -1);

    // act
    // player attempts to discard zero cards
    ret = ambassadorRefactor(2, 0, &state, 0);

    // assert
    assertEqual("[MY BUG] Attempting to discard 0 copies of a card is allowed", ret, 0);

    //arrange
    state.handCount[currentPlayer] = 4;
    state.hand[currentPlayer][0] = ambassador;
    state.hand[currentPlayer][1] = minion;
    state.hand[currentPlayer][2] = estate;
    state.hand[currentPlayer][3] = silver;

    // act
    // player attempts to discard estate, but specifies 2 when the player only has 1 estate to give
    ret = ambassadorRefactor(2, 2, &state, 0);

    // assert
    // BUG: the game lets this happen, it "generates" copies of the card the player is attempting to discard
    // if the player is trying to discard more than they actually own
    assertEqual("[MY BUG] Attempting to discard 2 copies of a card the player only has 1 of is rejected", ret, -1);

    //assert
    // test that the code returns an error when the player selects a card not in their hand? (UI may not allow it)
    ret = ambassadorRefactor(3, 2, &state, 0);
    assertEqual("[MY BUG] Attempting to discard a card the player doesn't have is rejected", ret, -1);


}

int main() {

    ambassadorTest1();

    printf("[Ambassador Card test] Test complete\n");
    return 0;
}
