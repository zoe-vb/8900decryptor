CC = gcc
INCLUDE = -I. -I/cygdrive/c/cygwin/usr/local/ssl/include 
#CFLAG = -Wall -O2 -funroll-loops
CFLAGS= $(INCLUDE) $(CFLAG)
TARGET = 8900decryptor
LDFLAGS = -lcrypto
OBJS = 8900decryptor.o

all: 8900decryptor

8900decryptor.o: 8900decryptor.c
	$(CC) $(CFLAGS) -c 8900decryptor.c

8900decryptor: 8900decryptor.o
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f *.o *.core $(TARGET)
