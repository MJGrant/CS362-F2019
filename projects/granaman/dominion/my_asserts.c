// myAsserts methods

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "my_asserts.h"

void assertEqual(char *testName, int param1, int param2) {
    if (param1 == param2) {
        printf("✔ -- %s [Passed] %d and %d are equal.\n", testName, param1, param2);
    } else {
        printf("x -- %s [FAILED!] %d and %d expected to be equal.\n", testName, param1, param2);
    }
}

void assertNotEqual(char *testName, int param1, int param2) {
    if (param1 != param2) {
        printf("✔ -- %s [Passed] %d and %d are not equal.\n", testName, param1, param2);
    } else {
        printf("x -- %s [FAILED!] %d and %d expected to be inequal.\n", testName, param1, param2);
    }
}
