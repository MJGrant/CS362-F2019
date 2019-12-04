CFLAGS= -Wall -fpic -coverage -lm -std=c99

rngs.o: rngs.h rngs.c
	gcc -c rngs.c -g  $(CFLAGS)

dominion.o: dominion.h dominion.c rngs.o
	gcc -c dominion.c -g  $(CFLAGS)

playdom: dominion.o playdom.c
	gcc -o playdom playdom.c -g dominion.o rngs.o $(CFLAGS)
#To run playdom you need to entere: ./playdom <any integer number> like ./playdom 10*/

testDrawCard: testDrawCard.c dominion.o rngs.o
	gcc  -o testDrawCard -g  testDrawCard.c dominion.o rngs.o $(CFLAGS)

interface.o: interface.h interface.c
	gcc -c interface.c -g  $(CFLAGS)

runtests: testDrawCard
	./testDrawCard &> unittestresult.out
	gcov dominion.c >> unittestresult.out
	cat dominion.c.gcov >> unittestresult.out

unittestresults: unittest_granaman.c unittest_tayloreb.c unittest_barwickl.c my_asserts.o dominion.o
	echo "unittest_granaman.c:" > unittestresults.out
	gcc -o unittest_granaman dominion.c rngs.c unittest_granaman.c my_asserts.c $(CFLAGS)
	./unittest_granaman >> unittestresults.out
	echo "unittest_tayloreb.c:" >> unittestresults.out
	gcc -o unittest_tayloreb dominion.c rngs.c unittest_tayloreb.c my_asserts.c $(CFLAGS)
	./unittest_tayloreb >> unittestresults.out
	echo "unittest_barwickl.c:" >> unittestresults.out
	gcc -o unittest_barwickl dominion.c rngs.c unittest_barwickl.c my_asserts.c $(CFLAGS)
	./unittest_barwickl >> unittestresults.out	
	echo "coverage" >> unittestresults.out
	gcov dominion.c -b -f >> unittestresults.out
	cat dominion.c.gcov >> unittestresults.out

player: player.c interface.o
	gcc -o player player.c -g  dominion.o rngs.o interface.o $(CFLAGS)

all: playdom player

clean:
	rm -f *.o playdom.exe playdom player player.exe  *.gcov *.gcda *.gcno *.so *.out testDrawCard testDrawCard.exe
