
CC = gcc
CFLAGS = -fPIC 

all:my_nmap

PortScanner_shared.o:
	$(CC) $(CFLAGS) -c PortScanner.c -o PortScanner_shared.o

libPortScanner.so: PortScanner_shared.o
	$(CC) -shared PortScanner_shared.o -o libPortScanner.so

my_nmap:my_nmap.c libPortScanner.so
	$(CC) my_nmap.c -o my_nmap -lPortScanner -L . -Wl,-rpath=.

.PHONY: clean

clean:
	rm -f my_nmap PortScanner_share.o libPortScanner.so