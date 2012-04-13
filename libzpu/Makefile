all:
	gcc -Wall -Werror -O2 -c -o zpu.o zpu.c
	ar cru libzpu.a zpu.o
	ranlib libzpu.a

clean:
	rm -f libzpu.a *.o
