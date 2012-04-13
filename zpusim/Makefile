TARGET=zpusim

all:
	gcc -O2 -g -Werror -Wall -o $(TARGET) $(TARGET).c util.c -L../libzpu -I../libzpu -lzpu

clean:
	rm -f $(TARGET)
