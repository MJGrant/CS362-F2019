#ifndef _MY_ASSERTS_H
#define _MY_ASSERTS_H

// myAsserts method signatures

void assertEqual(char *testName, int param1, int param2);
void assertNotEqual(char *testName, int param1, int param2);
void assertIncreasedByOne(char *testName, int before, int after);
void assertDecreasedByOne(char *testName, int before, int after);

#endif
