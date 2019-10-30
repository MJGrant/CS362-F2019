#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

char inputChar()
{
    // My inputs include all upper- and lower-case letters, numbers, symbols such as !, ~, and @, and space. Basically,
    // if it's on a typical North American keyboard then it's a valid char for the purposes of this quiz.

    // Those valid ASCII values are numbered 32-126

    // Pick a random number between 0 and 94.
    // Add +32 at the end to shift the range to 32-126

    char randomChar = (rand() % 95) + 32;

    return randomChar;
}

char *inputString()
{
    // The test ends when this happens to spell out "reset"
    // We can make that more likely by limiting the possible character set to r, e, s, e, t
    // e is twice as likely to appear than other letters because it appears twice in the word

    char letters[5] = {'r', 'e', 's', 'e', 't'}; // one extra char to account for \0
    char str[6];

    for (int i = 0; i < 5; i++) {
        int randomIndex = (rand()%5); // draws a random number between 0 and 4 inclusive
        printf("Random index is: %d , corresponds to random letter: %c \n", randomIndex, letters[randomIndex]);
        str[i] = 'a';
    }
    //str[0] = 'a';
    //str[1] = 'b';
    //str[2] = 'c';
    //str[3] = 'd';
    //str[4] = 'e';
    //str[5] = 'f';
    //str[6] = '\0';

    printf("string: %s \n", str);
    return str;
}

void testme()
{
  int tcCount = 0;
  char *s;
  char c;
  int state = 0;
  while (1)
  {
    tcCount++;
    c = inputChar();
    s = inputString();
    //printf("Iteration %d: c = %c, s = %s, state = %d\n", tcCount, c, s, state);

    if (c == '[' && state == 0) state = 1;
    if (c == '(' && state == 1) state = 2;
    if (c == '{' && state == 2) state = 3;
    if (c == ' '&& state == 3) state = 4;
    if (c == 'a' && state == 4) state = 5;
    if (c == 'x' && state == 5) state = 6;
    if (c == '}' && state == 6) state = 7;
    if (c == ')' && state == 7) state = 8;
    if (c == ']' && state == 8) state = 9;
    if (s[0] == 'r' && s[1] == 'e'
       && s[2] == 's' && s[3] == 'e'
       && s[4] == 't' && s[5] == '\0'
       && state == 9)
    {
      printf("error ");
      exit(200);
    }
  }
}


int main(int argc, char *argv[])
{
    srand(time(NULL));
    testme();
    return 0;
}
