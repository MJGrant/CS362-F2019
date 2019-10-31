CFLAGS = -Wall -fpic -coverage -lm

testme:
	gcc -o testme -g testme.c $(CFLAGS)
	./testme

clean:
	rm -f testme *.gcov *.gcda *.gcno *.o *.out
