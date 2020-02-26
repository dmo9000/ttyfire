
all: ttyfire

ttyfire: ttyfire.c
	$(CC) -Wall -Werror -O2 -o ttyfire ttyfire.c -ltermcap

clean:
	rm -f ttyfire
