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

    char letters[10] = {'e','r','s','t','a','a','a','a','a','z'};
    static char str[6]; // build this string out of randomly chosen letters

    for (int i = 0; i < 5; i++) {
        str[i] = letters[rand() %10];
    }

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
    printf("Iteration %d: c = %c, s = %s, state = %d\n", tcCount, c, s, state);

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
