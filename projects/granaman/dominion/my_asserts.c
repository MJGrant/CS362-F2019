// myAsserts methods

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "my_asserts.h"

void printTestName(char *testName, char *testString) {
    printf("[%s] - %s\n", testName, testString);
}

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

// The tests frequently verify that a value was increased by 1
void assertIncreasedByOne(char *testName, int before, int after) {
    if (after - before == 1) {
        printf("✔ -- %s [Passed] %d is one greater than %d.\n", testName, after, before);
    } else {
        printf("x -- %s [FAILED!] %d was expected to be one greater than %d.\n", testName, after, before);
    }
}

// The tests frequently verify that a value was decreased by 1
void assertDecreasedByOne(char *testName, int before, int after) {
    if (before - after == 1) {
        printf("✔ -- %s [Passed] %d is one less than %d.\n", testName, after, before);
    } else {
        printf("x -- %s [FAILED!] %d was expected to be one less than %d.\n", testName, after, before);
    }
}

void assertAtLeast(char *testName, int atLeastThisMuch, int valueToCheck) {
    if (valueToCheck >= atLeastThisMuch) {
        printf("✔ -- %s [Passed] %d is greater than or equal to %d.\n", testName, valueToCheck, atLeastThisMuch);
    } else if (valueToCheck < atLeastThisMuch) {
        printf("x -- %s [FAILED!] %d was expected to be at least %d.\n", testName, valueToCheck, atLeastThisMuch);
    }

}
