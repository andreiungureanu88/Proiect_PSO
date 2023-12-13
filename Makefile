
CC = gcc

all:PortScanner
	gcc PortScanner.c -o PortScanner

.PHONY: clean

clean:
	rm PortScanner