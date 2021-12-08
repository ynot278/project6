CC = gcc
CFLAGS = 

all: oss userProcess

oss: main.c oss.h
	$(CC) $(CFLAGS) main.c -o oss

user: userProcess.c oss.h
	$(CC) $(CFLAGS) userProcess.c -o userProcess

clean:
	rm -rf oss userProcess *.txt