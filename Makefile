CC = gcc
INCLUDE = -I. -I/cygdrive/c/cygwin/usr/local/ssl/include 
#CFLAG = -Wall -O2 -funroll-loops
CFLAGS= $(INCLUDE) $(CFLAG)
TARGET = decryptor
LDFLAGS = -lcrypto
OBJS = decryptor.o

all: decryptor

decryptor.o: decryptor.c
	$(CC) $(CFLAGS) -c decryptor.c

decryptor: decryptor.o
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f *.o *.core $(TARGET)
