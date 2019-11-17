CC=gcc 
CFLAGS= -Wall -O2
CFLAGS= -Wall -Wno-unused 

all: serial

serial: serial.o 
	$(CC) -o $@ serial.o 
serial.o: serial.c 
	$(CC) $(CFLAGS) -c serial.c 

clean:
	rm -f serial *.o core
