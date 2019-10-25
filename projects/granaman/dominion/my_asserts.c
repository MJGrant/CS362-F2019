// myAsserts methods

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "my_asserts.h"

void assertEqual(int param1, int param2) {
    if (param1 == param2) {
        printf("passertEqual passed!");
    } else {
        printf("assertEqual failed! Params %d and %d expected to be equal.\n", param1, param2);
    }
}

void assertNotEqual(int param1, int param2) {
    if (param1 != param2) {
        printf("assertNotEqual passed!");
    } else {
        printf("assertNotEqual failed! Params %d and %d expected to be unequal.\n", param1, param2);
    }
}
