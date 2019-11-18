#ifndef _MY_ASSERTS_H
#define _MY_ASSERTS_H

// myAsserts method signatures
void printTestName(char *testName, char *testString);

void assertEqual(char *testName, int param1, int param2);
void assertNotEqual(char *testName, int param1, int param2);
void assertIncreasedByOne(char *testName, int before, int after);
void assertDecreasedByOne(char *testName, int before, int after);
void assertAtLeast(char *testName, int atLeastThisMuch, int valueToCheck);

#endif
